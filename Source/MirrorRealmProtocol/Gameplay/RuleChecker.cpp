#include "MirrorRealmProtocol/Gameplay/RuleChecker.h"

#include "Interpreter.h"
#include "MirrorRealmProtocol/Gameplay/MainServer.h"
#include "MirrorRealmProtocol/Gameplay/GameStruct.h"

bool URuleChecker::PlayCardLegal(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index) const
{
	if (Server -> Interpreter -> SolvingEffect.IsValid()) return false;
	
	const FCardInfo* CardInfo = Server -> GetCardInstCopybyID(CardInstID);
	
	if (!CardInfo) return false;
	
	if (CardInfo -> CardType == ECardType::Servant) return PlayServantLegal(PlayerID, CardInstID, GridOwner, Index);
	
	if (CardInfo -> CardType == ECardType::Spell)  return PlaySpellLegal(PlayerID, CardInstID, GridOwner, Index);

	return false;
}

bool URuleChecker::PlayServantLegal(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index) const
{
	//卡牌存在
	const FCardInfo* CardInfo = Server -> GetCardInstCopybyID(CardInstID);
	if (!CardInfo) return false;

	//位置检查
	if (CardInfo -> Location != ELocation::Hand) return false;
	
	//回合检查
	if (Server -> TurnOwner != PlayerID) return false;

	//拥有者检查
	if (CardInfo -> PlayerID != PlayerID) return false;
	if (GridOwner != PlayerID) return false;

	//地块检查
	const TArray<FIntPoint> Grids = {
		FIntPoint(0,0),
		FIntPoint(0,1),
		FIntPoint(0,2),
		FIntPoint(0,3),
		FIntPoint(1,0),
		FIntPoint(1,1),
		FIntPoint(1,2),
		FIntPoint(1,3)};
	if (!Grids.Contains(Index)) return false;

	//覆盖检查
	if (Server -> GetCardInstCopybyHex(CardInfo -> PlayerID, Index)) return false;

	//费用检查
	if ((CardInfo -> PlayerID == 0 ? Server -> Player_0_Mana : Server -> Player_1_Mana) < CardInfo -> RealCost + Index.X) return false;

	return true;
}

bool URuleChecker::PlaySpellLegal(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index)  const
{
	//卡牌存在
	const FCardInfo* CardInfo = Server -> GetCardInstCopybyID(CardInstID);
	if (!CardInfo) return false;

	//位置检查
	if (CardInfo -> Location != ELocation::Hand) return false;
	
	//回合检查
	if (Server -> TurnOwner != PlayerID) return false;

	//拥有者检查
	if (CardInfo -> PlayerID != PlayerID) return false;

	//费用检查
	if ((CardInfo -> PlayerID == 0 ? Server -> Player_0_Mana : Server -> Player_1_Mana) < CardInfo -> RealCost) return false;
	
	return true;
}

bool URuleChecker::AttackLegal(const int PlayerID, const int SourceCardInstID, const int TargetCardInstID) const
{
	if (Server -> Interpreter -> SolvingEffect.IsValid()) return false;
	
	//卡牌存在
	const FCardInfo* Attacker = Server -> GetCardInstCopybyID(SourceCardInstID);
	if (!Attacker) return false;
	
	const FCardInfo* Defender = Server -> GetCardInstCopybyID(TargetCardInstID);
	if (!Defender) return false;

	// 行动力检查
	if (!Attacker -> Action) return false;
	
	//位置检查
	if (Attacker -> Location != ELocation::Board) return false;
	if (Defender -> Location != ELocation::Board) return false;
	
	//回合检查
	if (Server -> TurnOwner != PlayerID) return false;

	//拥有者检查
	if (Attacker -> PlayerID != PlayerID) return false;
	if (Defender -> PlayerID == PlayerID) return false;

	//生命力检查
	if (Attacker ->RealHP <= 0) return false;
	if (Defender ->RealHP <= 0) return false;

	//同名检查
	if (SourceCardInstID == TargetCardInstID) return false;
	
	return true;
}

bool URuleChecker::AttackDirectlyLegal(const int PlayerID, const int SourceCardInstID, const int TargetPlayerID) const
{
	if (Server -> Interpreter -> SolvingEffect.IsValid()) return false;
	
	//卡牌存在
	const FCardInfo* CardInfo = Server -> GetCardInstCopybyID(SourceCardInstID);
	if (!CardInfo) return false;

	// 行动力检查
	if (!CardInfo -> Action) return false;
	
	//位置检查
	if (CardInfo -> Location != ELocation::Board) return false;
	
	//回合检查
	if (Server -> TurnOwner != PlayerID) return false;

	//拥有者检查
	if (CardInfo -> PlayerID != PlayerID) return false;
	if (PlayerID == TargetPlayerID) return false;
	
	return true;
}

bool URuleChecker::ActivateCardLegal(const int PlayerID, const int CardInstID)
{
	if (Server -> Interpreter -> SolvingEffect.IsValid()) return false;
	
	//卡牌存在
	const FCardInfo* CardInfo = Server -> GetCardInstCopybyID(CardInstID);
	if (!CardInfo) return false;

	// 行动力检查
	if (!CardInfo -> Action && CardInfo -> CardType == ECardType::Servant) return false;
	
	//位置检查
	if (CardInfo -> Location != ELocation::Board && CardInfo -> CardType == ECardType::Servant) return false;
	
	//回合检查
	if (Server -> TurnOwner != PlayerID) return false;

	//拥有者检查
	if (CardInfo -> PlayerID != PlayerID) return false;

	//对象检查
	if (CardInfo -> CardType == ECardType::Spell)
	{
		const FCardEffect* CardEffect = CardInfo -> CardEffects.FindByPredicate([this](const FCardEffect& Item) {return Item.CardEffectType == ECardEffectType::Spell;});
		if (!FindTargets(*CardInfo, *CardEffect)) return false;
	}

	if (CardInfo -> CardType == ECardType::Servant)
	{
		const FCardEffect* CardEffect = CardInfo -> CardEffects.FindByPredicate([this](const FCardEffect& Item) {return Item.CardEffectType ==ECardEffectType::Activate;});
		if (!FindTargets(*CardInfo, *CardEffect)) return false;
	}
	return true;
}

bool URuleChecker::FindTargets(const FCardInfo& CardInfo, const FCardEffect& CardEffect) const
{

	if (!CardEffect.IsValid()) return false;
	if (CardEffect.CardEffectTargets.Num() == 0) return true;
	for (FCardEffectTarget Emt : CardEffect.CardEffectTargets)
	{
		TArray<int> TargetInstIDs;
		for (int Idx : Server -> FindTargets(CardInfo.CardInstID, Emt))
		{
			TargetInstIDs.AddUnique(Idx);
		}
		if (TargetInstIDs.IsEmpty())
		{
			return false;
		}
	}
	return true;
}

bool URuleChecker::FindQueueTargets(const FCardInfo& CardInfo, const FQueueEffect& CardEffect) const
{
	if (!CardEffect.IsValid()) return false;
	if (CardEffect.CardEffectTargets.Num() == 0) return true;
	for (FCardEffectTarget Emt : CardEffect.CardEffectTargets)
	{
		TArray<int> TargetInstIDs;
		for (int Idx : Server -> FindTargets(CardInfo.CardInstID, Emt))
		{
			TargetInstIDs.AddUnique(Idx);
		}
		if (TargetInstIDs.Num() < Emt.Count)
		{
			return false;
		}
	}
	return true;
}