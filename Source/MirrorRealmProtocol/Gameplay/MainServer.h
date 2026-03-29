#pragma once

#include "CoreMinimal.h"
#include "GameStruct.h"
#include "GameFramework/GameState.h"
#include "MainServer.generated.h"

enum class EGamePhase : uint8;
struct FCardInfo;
struct FCardPreInfo;
class URuleChecker;
class UInterpreter;

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API AMainServer : public AGameState
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
public:	

	//组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInterpreter> InterpreterClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInterpreter* Interpreter = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<URuleChecker> RuleCheckerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URuleChecker* RuleChecker = nullptr;
	
	//总卡组
	UPROPERTY(ReplicatedUsing = OnRep_Decks, EditAnywhere, BlueprintReadWrite)
	TArray<FCardInfo> Decks = {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DeckSize = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ReadyClient = 0;
	
	UFUNCTION()
	void OnRep_Decks() const;

	UFUNCTION()
	void CopyDeckFromClient(const int PlayerID, const TArray<int>& CardPreInfo);
	
	UFUNCTION()
	static FCardInfo ConvertToInGameInfo(const FCardPreInfo& CardPreInfo);
	
	UFUNCTION()
	void Shuffle();

	UFUNCTION()
	void CheckClientsReady();

	//数据库
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* CardPreInfoLib;
	
	//玩家信息
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int Player_0_Mana = 1;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int Player_1_Mana = 1;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int Player_0_MaxMana = 1;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int Player_1_MaxMana = 1;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int Player_0_HP = 20;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int Player_1_HP = 20;

	//对局信息
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	EGamePhase GamePhase = EGamePhase::Awaiting;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	ETurnPhase TurnPhase = ETurnPhase::PlaceHolder;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int TurnNumber = INT_MIN;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int TurnOwner = INT_MIN;

	//对局流程
	UFUNCTION()
	void PreStartGame();
	
	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void PreStartTurn();
	
	UFUNCTION()
	void StartTurn();

	UFUNCTION()
	void PreEndTurn();
	
	UFUNCTION()
	void EndTurn();

	//基于标识符获取卡牌信息
	FCardInfo* GetCardInstRefbyID(int CardInstID);	//修改
	FCardInfo* GetCardInstRefbyHex(int PlayerID, FIntPoint Position);	//修改
	const FCardInfo* GetCardInstCopybyID(int CardInstID); //只读
	const FCardInfo* GetCardInstCopybyHex(int PlayerID, FIntPoint Position); //只读
	TArray<int> FindTargets(const int CardInstID, const FCardEffectTarget& CardEffectTarget);
	bool CheckTargets(const int CardInstID, const FCardInfo& CardInfo, const FCardEffectTarget& CardEffectTarget);

	//分配视觉与声效事件
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 GlobalEventID;
	
	UFUNCTION(NetMulticast, Reliable)
	void DispatchEvent(const FOrderUpdateEvent NewEvent);

	UFUNCTION(NetMulticast, Reliable)
	void DispatchFulfillTargetsEvent(const int PlayerID, const TArray<FCardEffectTarget>& CardEffectTargets);
	
	//服务器输入事件
	UFUNCTION()
	void RequestEndTurn(const int PlayerID);
	
	UFUNCTION()
	void RequestPlayCard(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index);

	UFUNCTION()
	void RequestAttack(const int PlayerID, const int SourceCardInstID, const int TargetCardInstID);

	UFUNCTION()
	void RequestAttackDirectly(const int PlayerID, const int SourceCardInstID, const int TargetPlayerID);

	UFUNCTION()
	void RequestActivateCard(const int PlayerID, const int CardInstID);

	UFUNCTION()
	void RequestFulfillTargets(const TArray<int>& TargetInstIDs);
};

