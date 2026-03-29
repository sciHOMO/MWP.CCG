#include "MirrorRealmProtocol/Gameplay/MainServer.h"

#include <rapidjson/reader.h>

#include "MirrorRealmProtocol/Gameplay/AudioManager.h"
#include "MirrorRealmProtocol/Gameplay/VisualManager.h"
#include "MirrorRealmProtocol/Gameplay/Interpreter.h"
#include "MirrorRealmProtocol/Gameplay/RuleChecker.h"
#include "MirrorRealmProtocol/Gameplay/GameController.h"
#include "MirrorRealmProtocol/Gameplay/GameStruct.h"
#include "Algo/RandomShuffle.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

void AMainServer::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority()) return;
	
	//创建解释器
	Interpreter = NewObject<UInterpreter>(this, InterpreterClass);
	check(Interpreter);
	Interpreter -> Server = this;
	
	//创建检查器
	RuleChecker = NewObject<URuleChecker>(this, RuleCheckerClass);
	check(RuleChecker);
	RuleChecker -> Server = this;
	
	//加载卡牌数据库
	CardPreInfoLib = LoadObject<UDataTable>(nullptr,TEXT("/Game/Misc/Library/CardPreInfoLib.CardPreInfoLib"));
}

void AMainServer::OnRep_Decks() const
{
	if (HasAuthority()) return;
	if (APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController())
	{
		if (AGameController* ThePC = Cast<AGameController>(PC))
		{
			ThePC -> CopyDeckFromServer();
		}
	}
}

void AMainServer::CopyDeckFromClient(const int PlayerID, const TArray<int>& CardPreInfo)
{
	if (CardPreInfo.Num() != DeckSize) return;
	if (!CardPreInfoLib) return;
	
	TArray<FCardInfo> TemporalDeck;
	for (const int Emt : CardPreInfo)
	{
		if (const FCardPreInfo* Row = CardPreInfoLib -> FindRow<FCardPreInfo>(FName(*FString::FromInt(Emt)), TEXT("Not Found")))
		{
			FCardInfo NewCardInfo = ConvertToInGameInfo(*Row);
			NewCardInfo.CardInstID = Decks.Num() + TemporalDeck.Num();
			NewCardInfo.PlayerID = PlayerID;
			NewCardInfo.Location = ELocation::Deck;
			NewCardInfo.Position = FIntPoint::NoneValue;
			TemporalDeck.Emplace(NewCardInfo);
		}
	}
	Decks.Append(TemporalDeck);
	Shuffle();
	Shuffle();
}

FCardInfo AMainServer::ConvertToInGameInfo(const FCardPreInfo& CardPreInfo)
{
	FCardInfo CardInfo;
	if (!CardPreInfo.IsValid()) return CardInfo; 
	CardInfo.CardID = CardPreInfo.CardID;
	CardInfo.Cost = CardPreInfo.Cost;
	CardInfo.RealCost = CardPreInfo.Cost;
	CardInfo.Name = CardPreInfo.Name;
	CardInfo.CardType = CardPreInfo.CardType;
	CardInfo.CardAttribute = CardPreInfo.CardAttribute;
	CardInfo.CardRace = CardPreInfo.CardRace;
	CardInfo.CardEffects = CardPreInfo.CardEffects;
	CardInfo.CardSpecs = CardPreInfo.CardSpecs;
	CardInfo.AP = CardPreInfo.AP;
	CardInfo.RealAP = CardPreInfo.AP;
	CardInfo.HP = CardPreInfo.HP;
	CardInfo.RealHP = CardPreInfo.HP;
	
	check(CardInfo.IsValid());
	return CardInfo;
}

void AMainServer::Shuffle()
{
	Algo::RandomShuffle(Decks);
}

void AMainServer::CheckClientsReady()
{
	ReadyClient++;
	if (ReadyClient == 2)
	{
		PreStartGame();
	}
}

void AMainServer::PreStartGame()
{
	GamePhase = EGamePhase::Start;
	TurnNumber = 0;
	TurnOwner = 0;
	StartGame();
}

void AMainServer::StartGame()
{
	if (Interpreter)
	{
	Interpreter -> DrawCard(0, 1);
	Interpreter -> DrawCard(1, 1);
	}
	PreStartTurn();
}

void AMainServer::PreStartTurn()
{
	GamePhase = EGamePhase::Continue;
	TurnPhase = ETurnPhase::PreStart;
	TurnNumber++;
	if (TurnNumber > 1)
	{
		TurnOwner = !TurnOwner;
	}
	
	FOrderUpdateEvent NewEvent;
	NewEvent.EventType = EEventType::Turn;
	NewEvent.IntInfo.Emplace(TurnOwner);
	NewEvent.GlobalEventID =GlobalEventID++;
	DispatchEvent(NewEvent);
	
	if (Interpreter)
	{
		Interpreter -> Revive(TurnOwner);
		TArray<int> TriggerSourceInfo;
		TriggerSourceInfo.Emplace(TurnOwner);
		Interpreter -> TriggerEffect(ECardEffectType::ListenStartTurn, TriggerSourceInfo);
		if (!Interpreter -> CacheQueue.IsEmpty() && !Interpreter -> QueueMuted)
		{
			Interpreter -> DequeueEffect();
		}
		else
		{
			StartTurn();
		}
	}
}

void AMainServer::StartTurn()
{
	TurnPhase = ETurnPhase::Start;
	TurnOwner == 0 ? Player_0_Mana = FMath::Max(Player_0_Mana + 1, Player_0_MaxMana) : Player_1_Mana = FMath::Max(Player_1_Mana + 1, Player_1_MaxMana);
	if (Interpreter)
	{
		//Interpreter -> DrawCard(TurnOwner, 1);
	}
}

void AMainServer::PreEndTurn()
{
	TurnPhase = ETurnPhase::PreEnd;
	if (Interpreter)
	{
		Interpreter -> Revive(TurnOwner);
		TArray<int> TriggerSourceInfo;
		TriggerSourceInfo.Emplace(TurnOwner);
		Interpreter -> TriggerEffect(ECardEffectType::ListenEndTurn, TriggerSourceInfo);
		if (! Interpreter -> CacheQueue.IsEmpty() && !Interpreter -> QueueMuted)
		{
			Interpreter -> DequeueEffect();
		}
		else
		{
			EndTurn();
		}
	}
}

void AMainServer::EndTurn()
{
	TurnPhase = ETurnPhase::End;
	PreStartTurn();
}

FCardInfo* AMainServer::GetCardInstRefbyID(int CardInstID)
{
	for (FCardInfo& Emt : Decks)
	{
		if (Emt.CardInstID == CardInstID)
		{
			return &Emt;
		}
	}
	return nullptr;
}

FCardInfo* AMainServer::GetCardInstRefbyHex(int PlayerID, FIntPoint Position)
{
	for (FCardInfo& Emt : Decks)
	{
		if (Emt.Position == Position && Emt.Location == ELocation::Board && Emt.PlayerID == PlayerID)
		{
			return &Emt;
		}
	}
	return nullptr;
}

const FCardInfo* AMainServer::GetCardInstCopybyID(int CardInstID)
{
	for (const FCardInfo& Emt : Decks)
	{
		if (Emt.CardInstID == CardInstID)
		{
			return &Emt;
		}
	}
	return nullptr;
}

const FCardInfo* AMainServer::GetCardInstCopybyHex(int PlayerID, FIntPoint Position)
{
	for (const FCardInfo& Emt : Decks)
	{
		if (Emt.Position == Position && Emt.Location == ELocation::Board && Emt.PlayerID == PlayerID)
		{
			return &Emt;
		}
	}
	return nullptr;
}

TArray<int> AMainServer::FindTargets(const int CardInstID, const FCardEffectTarget& CardEffectTarget)
{
	TArray<int> FoundTargets;
	for (const FCardInfo& CardInfo : Decks)
	{
		if (CheckTargets(CardInstID, CardInfo, CardEffectTarget))
		{
			FoundTargets.Emplace(CardInfo.CardInstID);
		}
	}
	return FoundTargets;
}

bool AMainServer::CheckTargets(const int CardInstID, const FCardInfo& CardInfo, const FCardEffectTarget& CardEffectTarget)
{
	if (CardEffectTarget.TargetType != ETargetType::Card) return true;
	
	if (CardEffectTarget.TeamType != ETeamType::PlaceHolder)
	{
		if (CardEffectTarget.TeamType == ETeamType::Owner && GetCardInstRefbyID(CardInstID) -> PlayerID != CardInfo.PlayerID) return false;
		if (CardEffectTarget.TeamType == ETeamType::Opponent && GetCardInstRefbyID(CardInstID) -> PlayerID == CardInfo.PlayerID) return false;
	}

	if (CardEffectTarget.CardType != ECardType::PlaceHolder && CardInfo.CardType != CardEffectTarget.CardType) return false;

	if (CardEffectTarget.CardLocation != ELocation::PlaceHolder && CardInfo.Location != CardEffectTarget.CardLocation) return false;
		
	if (CardEffectTarget.CardAttribute != ECardAttribute::PlaceHolder && CardInfo.CardAttribute != CardEffectTarget.CardAttribute)return false;
	
	if (CardEffectTarget.CardRace != ECardRace::PlaceHolder && CardInfo.CardRace != CardEffectTarget.CardRace) return false;
	
	if (CardInfo.RealCost > CardEffectTarget.MaxCP || CardInfo.RealCost < CardEffectTarget.MinCP) return false;

	if (CardInfo.RealHP > CardEffectTarget.MaxHP || CardInfo.RealHP < CardEffectTarget.MinHP) return false;

	if (CardInfo.RealAP > CardEffectTarget.MaxAP || CardInfo.RealAP < CardEffectTarget.MinAP) return false;

	return true;
}

void AMainServer::DispatchEvent_Implementation(const FOrderUpdateEvent NewEvent)
{
	if (HasAuthority()) return;
	if (const AGameController* PC = Cast<AGameController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		if (PC -> VisualManager)
		{
			PC -> VisualManager -> ReceiveEvent(NewEvent);
		}
		if (PC -> AudioManager)
		{
			PC -> AudioManager -> ReceiveEvent(NewEvent);
		}
	}
}

void AMainServer::DispatchFulfillTargetsEvent_Implementation(const int PlayerID, const TArray<FCardEffectTarget>& CardEffectTargets)
{
	if (HasAuthority()) return;
	if (AGameController* PC = Cast<AGameController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)); PlayerID == PC -> PlayerState -> GetPlayerId())
	{
		PC ->  ReceiveFulfillEvent(CardEffectTargets);
	}
}

void AMainServer::RequestEndTurn(const int PlayerID)
{
	if (TurnOwner == PlayerID)
	{
		PreEndTurn();
	}
}

void AMainServer::RequestPlayCard(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index)
{
	const FCardInfo* Card = GetCardInstRefbyID(CardInstID);
	if (Interpreter && RuleChecker -> PlayCardLegal(PlayerID, CardInstID, GridOwner, Index) && Card)
	{
		Interpreter -> CardMove(CardInstID, INT_MAX, ELocation::Hand, Card -> CardType == ECardType::Servant ? ELocation::Board : ELocation::Grave, FIntPoint::NoneValue, Index);
		if (Card -> CardType == ECardType::Spell)
		{
			Interpreter -> CardActivate(CardInstID);
		}
	}
}

void AMainServer::RequestAttack(const int PlayerID, const int SourceCardInstID, const int TargetCardInstID)
{
	const FCardInfo* Card_1 = GetCardInstRefbyID(SourceCardInstID);
	const FCardInfo* Card_2 = GetCardInstRefbyID(TargetCardInstID);
	if (Interpreter && RuleChecker -> AttackLegal(PlayerID, SourceCardInstID,TargetCardInstID) && Card_1 && Card_2)
	{
		Interpreter -> CardAttack(SourceCardInstID, TargetCardInstID);
	}
}

void AMainServer::RequestAttackDirectly(const int PlayerID, const int SourceCardInstID, const int TargetPlayerID)
{
	const FCardInfo* Card_1 = GetCardInstRefbyID(SourceCardInstID);

	if (Interpreter && RuleChecker -> AttackDirectlyLegal(PlayerID, SourceCardInstID, TargetPlayerID) && Card_1)
	{
		Interpreter -> CardAttackDirectly(SourceCardInstID, TargetPlayerID);
	}
}

void AMainServer::RequestActivateCard(const int PlayerID, const int CardInstID)
{
	const FCardInfo* Card = GetCardInstRefbyID(CardInstID);
	if (Interpreter && RuleChecker -> ActivateCardLegal(PlayerID, CardInstID) && Card)
	{
		Interpreter -> CardActivate(CardInstID);
	}
}

void AMainServer::RequestFulfillTargets(const TArray<int>& TargetInstIDs)
{
	if (Interpreter)
	{
		Interpreter -> SolvingEffect.CardInstTargets = TargetInstIDs;
		Interpreter -> CheckEffect();
	}
}

void AMainServer::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME( AMainServer, Decks);
	DOREPLIFETIME( AMainServer, Player_0_Mana);
	DOREPLIFETIME( AMainServer, Player_1_Mana);
	DOREPLIFETIME( AMainServer, Player_0_MaxMana);
	DOREPLIFETIME( AMainServer, Player_1_MaxMana);
	DOREPLIFETIME( AMainServer, Player_0_HP);
	DOREPLIFETIME( AMainServer, Player_1_HP);
	DOREPLIFETIME( AMainServer, GlobalEventID);
}


