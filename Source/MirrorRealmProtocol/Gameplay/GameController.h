#pragma once

#include "CoreMinimal.h"
#include "GameStruct.h"
#include "GameFramework/PlayerController.h"
#include "GameController.generated.h"

struct FCardEffectTarget;
class AMainServer;
class UVisualManager;
class UAudioManager;

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API AGameController : public ANoPawnPlayerController
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PostSeamlessTravel() override;
	
	// 组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AMainServer* Server;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UVisualManager> VisualManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVisualManager* VisualManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAudioManager> AudioManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioManager* AudioManager;

	UFUNCTION(NetMulticast, Reliable)
	void BothLoadComponent();
	
	UFUNCTION()
	void LoadComponent();

	//游戏输入

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FCardEffectTarget> CardEffectTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> TargetInstIDs;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CardEffectTargetIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* FirstPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* FirstReleased;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SecondPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* SecondReleased;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnFirstPress();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnFirstRelease();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSecondPress();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnSecondRelease();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnClickTarget();
	
	//游戏请求
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RequestEndTurn(const int PlayerID);
	
	UFUNCTION(Server, Reliable)
	void RequestPlayCard(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index);

	UFUNCTION(Server, Reliable)
	void RequestAttack(const int PlayerID, const int SourceCardInstID, const int TargetCardInstID);

	UFUNCTION(Server, Reliable)
	void RequestAttackDirectly(const int PlayerID, const int SourceCardInstID, const int TargetPlayerID);

	UFUNCTION(Server, Reliable)
	void RequestActivateCard(const int PlayerID, const int CardInstID);
	
	UFUNCTION(Server, Reliable)
	void RequestFulfillTargets(const TArray<int>& TIDs);
	
	//游戏流程
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DeckName;

	UFUNCTION()
	void TrySendDeckToServer();
	
	UFUNCTION(Server, Reliable)
	void SendDeckToServer(const int PlayerID, const TArray<int>& CardPreInfo);
	
	UFUNCTION(Client, Reliable)
	void CopyDeckFromServer();

	UFUNCTION(Server, Reliable)
	void ClientControllerReady();
	
	UFUNCTION()
	void ReceiveFulfillEvent(const TArray<FCardEffectTarget>& CET);

	UFUNCTION()
		void CheckAndPass(AActor* FoundActor);
};
