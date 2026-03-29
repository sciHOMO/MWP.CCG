#pragma once

#include "CoreMinimal.h"
#include "MirrorRealmProtocol/Gameplay/GameStruct.h"
#include "Interpreter.generated.h"

enum class ELocation : uint8;
enum class ECardSpec : uint8;
struct FCardInfo;
struct FQueueEffect;
class AMainServer;

UCLASS()
class MIRRORREALMPROTOCOL_API UInterpreter : public UObject
{
	GENERATED_BODY()

	UInterpreter();
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="OMP_Interpreter")
	AMainServer* Server;

	//游戏效果处理
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FQueueEffect SolvingEffect;
	
	TArray<FQueueEffect> ListenEntryArray;
	TArray<FQueueEffect> ListenLeaveArray;
	TArray<FQueueEffect> ListenSpellArray;
	TArray<FQueueEffect> ListenStartTurnArray;
	TArray<FQueueEffect> ListenEndTurnArray;
	TArray<FQueueEffect> AuraArray;
	TQueue<FQueueEffect> CacheQueue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool QueueMuted = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Waiting = false;
	
	TMap<FName, void (UInterpreter::*)(const int, const TArray<int>&, const TArray<int>&)> FuncLib;

	UFUNCTION()
	void RegisterListenQueue(const int CardInstID);

	UFUNCTION()
	void UnregisterListenQueue(const int CardInstID);

	UFUNCTION()
	static FQueueEffect ConvertCardToQueue(const int CardInstID, const FCardEffect& CardEffect);

	UFUNCTION()
	void TriggerEffect(ECardEffectType EffectType, const TArray<int>& TriggerSourceInfo);
	
	UFUNCTION()
	void DequeueEffect();

	UFUNCTION()
	void CheckEffect();

	UFUNCTION()
	void ContinueEffect();

	UFUNCTION()
	bool ProcessEffect(const int CardInstID, const FName CardEffectFunction, const TArray<int>& CardInstTargets, const TArray<int>& TriggerSourceInfo);
	
	//基本事件
	UFUNCTION()
	void DrawCard(const int PlayerID, const int Count);

	UFUNCTION()
	void Revive(const int PlayerID) const;
	
	//元事件
	UFUNCTION()
	void CardMove(const int CardInstID, const int SourceCardInstID, const ELocation StartLocation, const ELocation EndLocation, const FIntPoint StartPosition, const FIntPoint EndPosition);

	UFUNCTION()
	void CardAttack(const int SourceCardInstID, const int TargetCardInstID);

	UFUNCTION()
	void CardAttackDirectly(const int SourceCardInstID, const int TargetPlayerID);
	
	UFUNCTION()
	void CardTakeDamage(const int SourceCardInstID, const int TargetCardInstID, const int DamageValue);

	UFUNCTION()
	void PlayerTakeDamage(const int SourceCardInstID, const int TargetPlayerID, const int DamageValue);
	
	UFUNCTION()
	void CardActivate(const int CardInstID);

	UFUNCTION()
	void CardInfluence(const int SourceCardInstID, const int TargetCardInstID, const TMap<ECardSpec, int> Affect);
	
	//效果模板函数
	UFUNCTION()
	void FuncTemplate(const int CardInstID, const TArray<int>& CardInstTargets, const TArray<int>& ExtraInfo);

	UFUNCTION()
	bool FuncTemplate_Check(const int CardInstID, const TArray<int>& CardInstTargets, const TArray<int>& ExtraInfo);

	UFUNCTION()
	void FuncTemplate_Process(const int CardInstID, const TArray<int>& CardInstTargets, const TArray<int>& ExtraInfo);

	//效果函数
	UFUNCTION()
	void BestowPlusOnePlusOne(const int CardInstID, const TArray<int>& CardInstTargets, const TArray<int>& ExtraInfo);
};
