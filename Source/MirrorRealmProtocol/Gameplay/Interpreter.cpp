#include "MirrorRealmProtocol/Gameplay/Interpreter.h"

#include "RuleChecker.h"
#include "MirrorRealmProtocol/Gameplay/GameStruct.h"
#include "MirrorRealmProtocol/Gameplay/MainServer.h"

UInterpreter::UInterpreter()
{
	Server = nullptr;
	FuncLib.Emplace(FName("FuncTemplate"), &UInterpreter::FuncTemplate);
	FuncLib.Emplace(FName("BestowPlusOnePlusOne"), &UInterpreter::BestowPlusOnePlusOne);
}

void UInterpreter::RegisterListenQueue(const int CardInstID)
{
	FCardInfo* CardInfo = Server -> GetCardInstRefbyID(CardInstID);
	if (!CardInfo) return;

	for (const FCardEffect& CardEffect : CardInfo -> CardEffects)
	{
		switch(CardEffect.CardEffectType)
		{
		case ECardEffectType::AuraType :
			{
				AuraArray.Emplace(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
				break;
			}	
		case ECardEffectType::ListenEntry :
			{
				ListenEntryArray.Emplace(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
				break;
			}
		case ECardEffectType::ListenLeave :
			{
				ListenLeaveArray.Emplace(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
				break;
			}
		case ECardEffectType::ListenSpell :
			{
				ListenSpellArray.Emplace(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
				break;
			}
		case ECardEffectType::ListenStartTurn :
			{
				ListenStartTurnArray.Emplace(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
				break;
			}
		case ECardEffectType::ListenEndTurn :
			{
				ListenEndTurnArray.Emplace(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
				break;
			}
		default : break;
		}
	}
}

void UInterpreter::UnregisterListenQueue(const int CardInstID)
{
	const FCardInfo* CardInfo = Server -> GetCardInstRefbyID(CardInstID);
	if (!CardInfo) return;
	
	const int TargetID = CardInfo -> CardInstID;
	AuraArray.RemoveAll([TargetID](const FQueueEffect& Effect){return Effect.CardInstID == TargetID;});
	ListenEntryArray.RemoveAll([TargetID](const FQueueEffect& Effect){return Effect.CardInstID == TargetID;});
	ListenLeaveArray.RemoveAll([TargetID](const FQueueEffect& Effect){return Effect.CardInstID == TargetID;});
	ListenSpellArray.RemoveAll([TargetID](const FQueueEffect& Effect){return Effect.CardInstID == TargetID;});
	ListenStartTurnArray.RemoveAll([TargetID](const FQueueEffect& Effect){return Effect.CardInstID == TargetID;});
	ListenEndTurnArray.RemoveAll([TargetID](const FQueueEffect& Effect){return Effect.CardInstID == TargetID;});
}

FQueueEffect UInterpreter::ConvertCardToQueue(const int CardInstID, const FCardEffect& CardEffect)
{
	FQueueEffect QueueEffect;
	if (!CardEffect.IsValid()) return QueueEffect;
	
	QueueEffect.CardInstID = CardInstID;
	QueueEffect.TriggerSource = CardEffect.TriggerSource;
	QueueEffect.TriggerSourceInfo = {};
	QueueEffect.CardEffectFunction = CardEffect.CardEffectFunction;
	QueueEffect.Counter = CardEffect.Counter;
	QueueEffect.CardEffectTargets = CardEffect.CardEffectTargets;
	QueueEffect.CardInstTargets = {};

	return QueueEffect;
}

void UInterpreter::TriggerEffect(ECardEffectType EffectType, const TArray<int>& TriggerSourceInfo)
{
	TArray<FQueueEffect>* FoundQueue = nullptr;
	switch (EffectType)
	{
	case ECardEffectType::ListenEntry :
		{
			FoundQueue = &ListenEntryArray;
			break;
		}
	case ECardEffectType::ListenLeave :
		{
			FoundQueue = &ListenLeaveArray;
			break;
		}
	case ECardEffectType::ListenSpell :
		{
			FoundQueue = &ListenSpellArray;
			break;
		}
	case ECardEffectType::ListenStartTurn :
		{
			FoundQueue = &ListenStartTurnArray;
			break;
		}
	case ECardEffectType::ListenEndTurn :
		{
			FoundQueue = &ListenEndTurnArray;
			break;
		}
	default : break;	
	}

	if (!FoundQueue) return;
	
	for (const FQueueEffect QueueEffect : *FoundQueue)
	{
		FQueueEffect NewQueueEffect = QueueEffect;
		NewQueueEffect.TriggerSourceInfo.Append(TriggerSourceInfo);
		CacheQueue.Enqueue(NewQueueEffect);
	}
}

void UInterpreter::DequeueEffect()
{
	if (FQueueEffect Output; CacheQueue.Dequeue(Output))
	{
		QueueMuted = true;
		SolvingEffect = Output;
		CheckEffect();
	}
}

void UInterpreter::CheckEffect()
{
	FCardInfo* CardInfo = Server -> GetCardInstRefbyID(SolvingEffect.CardInstID);
	if (!CardInfo) return;

	if (! Server -> RuleChecker -> FindQueueTargets(*CardInfo, SolvingEffect))
	{
		GetWorld() -> GetTimerManager().SetTimerForNextTick(this, &UInterpreter::ContinueEffect);
		QueueMuted = false;
		return;
	}
	
	if (SolvingEffect.CardEffectTargets.Num() == SolvingEffect.CardInstTargets.Num())
	{
		FOrderUpdateEvent NewEvent;
		NewEvent.EventType = EEventType::Activate;
		NewEvent.IntInfo.Emplace(SolvingEffect.CardInstID);
		NewEvent.SCardInfo.Emplace(*CardInfo);
		NewEvent.GlobalEventID = Server -> GlobalEventID++;
		Server -> DispatchEvent(NewEvent);
		
		if (ProcessEffect(SolvingEffect.CardInstID, SolvingEffect.CardEffectFunction, SolvingEffect.CardInstTargets, SolvingEffect.TriggerSourceInfo))
		{
			SolvingEffect = FQueueEffect();
			GetWorld() -> GetTimerManager().SetTimerForNextTick(this, &UInterpreter::ContinueEffect);
			QueueMuted = false;
		}
	}
	else
	{
		Server -> DispatchFulfillTargetsEvent(CardInfo -> PlayerID, SolvingEffect.CardEffectTargets);
	}
}

void UInterpreter::ContinueEffect()
{
	if (!CacheQueue.IsEmpty() && !QueueMuted)
	{
		DequeueEffect();
		return;
	}
	if (CacheQueue.IsEmpty() && QueueMuted && Waiting)
	{
		if (Server -> TurnPhase == ETurnPhase::PreStart)
		{
			Server -> StartTurn();
		}
		if (Server -> TurnPhase == ETurnPhase::PreEnd)
		{
			Server -> EndTurn();
		}
	}
}

bool UInterpreter::ProcessEffect(const int CardInstID, const FName CardEffectFunction, const TArray<int>& CardInstTargets, const TArray<int>& TriggerSourceInfo)
{
	if (const auto* FuncPtr = FuncLib.Find(CardEffectFunction))
	{
		(this ->**FuncPtr) (CardInstID, CardInstTargets, TriggerSourceInfo);
		return true;
	}
	return false;
}

void UInterpreter::DrawCard(const int PlayerID, const int Count)
{
	for (int i = 0; i < Count; ++i)
	{
		const FCardInfo* CardToDraw = nullptr;
		for (FCardInfo& Card : Server -> Decks)
		{
			if (Card.PlayerID == PlayerID && Card.Location == ELocation::Deck)
			{
				CardToDraw = &Card;
				break;
			}
		}
		if (CardToDraw)
		{
			CardMove(CardToDraw -> CardInstID, INT_MAX, ELocation::Deck, ELocation::Hand, FIntPoint::NoneValue, FIntPoint::NoneValue);
		}
	}
}

void UInterpreter::Revive(const int PlayerID) const
{
	const TArray<FIntPoint> Grids = {
		FIntPoint(0,0),
		FIntPoint(0,1),
		FIntPoint(0,2),
		
		FIntPoint(0,3),
		FIntPoint(1,0),
		FIntPoint(1,1),
		FIntPoint(1,2),
		FIntPoint(1,3)};

	for (FIntPoint Emt : Grids)
	{
		if (FCardInfo* FoundCard = Server -> GetCardInstRefbyHex(PlayerID, Emt)) FoundCard -> Action = true;
	}
}

void UInterpreter::CardMove(const int CardInstID, const int SourceCardInstID, const ELocation StartLocation, const ELocation EndLocation, const FIntPoint StartPosition, const FIntPoint EndPosition)
{
	FCardInfo* CardInfo = Server -> GetCardInstRefbyID(CardInstID);
	if (!CardInfo) return;
	
	CardInfo -> Location = EndLocation;
	CardInfo -> Position = EndPosition;

	if (StartLocation == ELocation::Hand && EndLocation == ELocation::Board && EndPosition.X == 1)
	{
		CardInfo -> Action = true;
	}
	
	FOrderUpdateEvent NewEvent;
	NewEvent.EventType = EEventType::Move;
	NewEvent.IntInfo.Emplace(CardInstID);
	NewEvent.IntInfo.Emplace(CardInfo -> PlayerID);
	NewEvent.LocationInfo.Emplace(StartLocation);
	NewEvent.LocationInfo.Emplace(EndLocation);
	NewEvent.PositionInfo.Emplace(StartPosition);
	NewEvent.PositionInfo.Emplace(EndPosition);
	NewEvent.SCardInfo.Emplace(*CardInfo);
	NewEvent.GlobalEventID = Server -> GlobalEventID++;
	Server -> DispatchEvent(NewEvent);
	
	if (StartLocation == ELocation::Hand && EndLocation == ELocation::Board)
	{
		for (const FCardEffect& CardEffect : CardInfo -> CardEffects)
		{
			if (CardEffect.CardEffectType == ECardEffectType::BattleCry)
			{
				CacheQueue.Enqueue(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
			}
		}
	}

	if (StartLocation == ELocation::Board && EndLocation == ELocation::Grave)
	{
		for (const FCardEffect& CardEffect : CardInfo -> CardEffects)
		{
			if (CardEffect.CardEffectType == ECardEffectType::DeathRattle)
			{
				CacheQueue.Enqueue(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
			}
		}
	}

	TArray<int> TriggerSourceInfo;
	TriggerSourceInfo.Emplace(CardInstID);
	
	if (EndLocation == ELocation::Board)
	{
		TriggerEffect(ECardEffectType::ListenEntry, TriggerSourceInfo);
	}
	if (CardInfo -> CardType == ECardType::Servant && EndLocation == ELocation::Grave)
	{
		TriggerEffect(ECardEffectType::ListenLeave, TriggerSourceInfo);
	}
	if (CardInfo -> CardType == ECardType::Spell && EndLocation == ELocation::Grave)
	{
		TriggerEffect(ECardEffectType::ListenSpell, TriggerSourceInfo);
	}

	if (CardInfo -> Location == ELocation::Board)
	{
		RegisterListenQueue(CardInstID);
	}
	else
	{
		UnregisterListenQueue(CardInstID);
	}
	
	if (!CacheQueue.IsEmpty() && !QueueMuted)
	{
		DequeueEffect();
	}
}

void UInterpreter::CardAttack(const int SourceCardInstID, const int TargetCardInstID)
{
	FCardInfo*Attacker = Server -> GetCardInstRefbyID(SourceCardInstID);
	const FCardInfo* Defender = Server -> GetCardInstRefbyID(TargetCardInstID);

	if (!Attacker || !Defender) return;

	const int AP_1 = Attacker -> RealAP;
	const int AP_2 = Defender -> RealAP;
	Attacker -> Action = false;
	
	FOrderUpdateEvent NewEvent;
	NewEvent.EventType = EEventType::Attack;
	NewEvent.IntInfo.Emplace(SourceCardInstID);
	NewEvent.IntInfo.Emplace(Attacker -> PlayerID);
	NewEvent.IntInfo.Emplace(TargetCardInstID);
	NewEvent.IntInfo.Emplace(Defender -> PlayerID);
	NewEvent.SCardInfo.Emplace(*Attacker);
	NewEvent.GlobalEventID = Server -> GlobalEventID++;
	Server -> DispatchEvent(NewEvent);
	
	CardTakeDamage(SourceCardInstID, TargetCardInstID, AP_1);
	CardTakeDamage(TargetCardInstID, SourceCardInstID, AP_2);
}

void UInterpreter::CardAttackDirectly(const int SourceCardInstID, const int TargetPlayerID)
{
	FCardInfo*Attacker = Server -> GetCardInstRefbyID(SourceCardInstID);
	if (!Attacker) return;

	const int AP = Attacker -> RealAP;
	Attacker -> Action = false;
	
	FOrderUpdateEvent NewEvent;
	NewEvent.EventType = EEventType::Attack;
	NewEvent.IntInfo.Emplace(SourceCardInstID);
	NewEvent.IntInfo.Emplace(Attacker -> PlayerID);
	NewEvent.IntInfo.Emplace(TargetPlayerID);
	NewEvent.SCardInfo.Emplace(*Attacker);
	NewEvent.GlobalEventID = Server -> GlobalEventID++;
	Server -> DispatchEvent(NewEvent);
	
	PlayerTakeDamage(SourceCardInstID, TargetPlayerID, AP);
}

void UInterpreter::CardTakeDamage(const int SourceCardInstID, const int TargetCardInstID, const int DamageValue)
{
	FCardInfo*Attacker = Server -> GetCardInstRefbyID(SourceCardInstID);
	FCardInfo* Defender = Server -> GetCardInstRefbyID(TargetCardInstID);

	if (!Attacker || !Defender) return;
	
	Defender -> RealHP = FMath::Max(0, Defender -> RealHP - DamageValue);
	
	FOrderUpdateEvent NewEvent;
	NewEvent.EventType = EEventType::Damage;
	NewEvent.IntInfo.Emplace(TargetCardInstID);
	NewEvent.IntInfo.Emplace(Defender -> PlayerID);
	NewEvent.IntInfo.Emplace(SourceCardInstID);
	NewEvent.IntInfo.Emplace(Attacker -> PlayerID);
	NewEvent.IntInfo.Emplace(DamageValue);
	NewEvent.SCardInfo.Emplace(*Defender);
	NewEvent.GlobalEventID = Server -> GlobalEventID++;
	Server -> DispatchEvent(NewEvent);
	
	if (Defender -> RealHP == 0)
	{
		CardMove(TargetCardInstID, SourceCardInstID, ELocation::Board, ELocation::Grave, Defender -> Position, FIntPoint::NoneValue);
	}
}

void UInterpreter::PlayerTakeDamage(const int SourceCardInstID, const int TargetPlayerID, const int DamageValue)
{
	FCardInfo*Attacker = Server -> GetCardInstRefbyID(SourceCardInstID);
	if (!Attacker) return;
	
	TargetPlayerID == 0 ? Server -> Player_0_HP = Server -> Player_0_HP - DamageValue : Server -> Player_1_HP = Server -> Player_1_HP - DamageValue;

	FOrderUpdateEvent NewEvent;
	NewEvent.EventType = EEventType::PlayerDamage;
	NewEvent.IntInfo.Emplace(SourceCardInstID);
	NewEvent.IntInfo.Emplace(Attacker -> PlayerID);
	NewEvent.IntInfo.Emplace(TargetPlayerID);
	NewEvent.GlobalEventID = Server -> GlobalEventID++;
	Server -> DispatchEvent(NewEvent);
}

void UInterpreter::CardActivate(const int CardInstID)
{
	FCardInfo* CardInfo = Server -> GetCardInstRefbyID(CardInstID);
	if (!CardInfo) return;

	CardInfo -> Action = false;

	FOrderUpdateEvent NewEvent;
	NewEvent.EventType = EEventType::Activate;
	NewEvent.IntInfo.Emplace(CardInstID);
	NewEvent.SCardInfo.Emplace(*CardInfo);
	NewEvent.GlobalEventID = Server -> GlobalEventID++;
	Server -> DispatchEvent(NewEvent);
	
	for (const FCardEffect& CardEffect : CardInfo -> CardEffects)
	{
		if (CardEffect.CardEffectType == ECardEffectType::Activate && CardInfo -> CardType == ECardType::Servant)
		{
			CacheQueue.Enqueue(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
		}
		if (CardEffect.CardEffectType == ECardEffectType::Spell && CardInfo -> CardType == ECardType::Spell)
		{
			CacheQueue.Enqueue(ConvertCardToQueue(CardInfo -> CardInstID, CardEffect));
		}
	}
	DequeueEffect();
}

void UInterpreter::CardInfluence(const int SourceCardInstID, const int TargetCardInstID, const TMap<ECardSpec, int> Affect)
{
	
}

void UInterpreter::FuncTemplate(const int CardInstID, const TArray<int>& CardInstTargets, const TArray<int>& ExtraInfo)
{
	const FCardInfo* CardInfo = Server -> GetCardInstRefbyID(CardInstTargets[0]);
	if (!CardInfo) return;
	
	DrawCard(CardInfo -> PlayerID, 2);
}

bool UInterpreter::FuncTemplate_Check(const int CardInstID, const TArray<int>& CardInstTargets, const TArray<int>& ExtraInfo)
{
	return false;
}

void UInterpreter::FuncTemplate_Process(const int CardInstID, const TArray<int>& CardInstTargets, const TArray<int>& ExtraInfo)
{
	
}

void UInterpreter::BestowPlusOnePlusOne(const int CardInstID, const TArray<int>& CardInstTargets, const TArray<int>& ExtraInfo)
{
	FCardInfo* CardInfo = Server -> GetCardInstRefbyID(CardInstTargets[0]);
	if (!CardInfo || CardInfo -> CardType != ECardType::Servant) return;

	if (CardInfo -> CardSpecs.Contains(ECardSpec::Talisman) && CardInfo -> CardSpecs[ECardSpec::Talisman] >= 1)
	{
		int Last = CardInfo -> CardSpecs[ECardSpec::Talisman];
		CardInfo -> CardSpecs.Emplace(ECardSpec::Talisman, --Last);
		return;
	}

	CardInfo -> RealAP++;
	CardInfo -> RealHP++;
	
	FOrderUpdateEvent NewEvent;
	NewEvent.EventType = EEventType::Update;
	NewEvent.IntInfo.Emplace(CardInstTargets[0]);
	NewEvent.SCardInfo.Emplace(*CardInfo);
	NewEvent.GlobalEventID = Server -> GlobalEventID++;
	Server -> DispatchEvent(NewEvent);
}



