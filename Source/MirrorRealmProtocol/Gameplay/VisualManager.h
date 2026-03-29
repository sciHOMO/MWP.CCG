#pragma once

#include "CoreMinimal.h"
#include "VisualManager.generated.h"

struct FOrderUpdateEvent;
class ABoardGrid;
class AGameController;
class ACardModel;

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API UVisualManager : public UObject
{
	GENERATED_BODY()

public:	

	//初始化
	UFUNCTION()
	void BeginPlay();

	UFUNCTION()
	void Tick();
	
	//接收事件
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LocalEventID = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FOrderUpdateEvent> Events;
	
	UFUNCTION()
	void ReceiveEvent(const FOrderUpdateEvent& NewEvent);
	
	UFUNCTION()
	void HandleEvent();
	
	UFUNCTION(BlueprintCallable)
	static void Test(const FOrderUpdateEvent& Event);

	//场景绑定
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGameController* Controller;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* CardModelLib;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntPoint, ABoardGrid*> GridMap;

	//卡牌移动事件
	UFUNCTION()
	void TurnChange(const FOrderUpdateEvent& Event);

	UFUNCTION()
	void ShowWinner(const FOrderUpdateEvent& Event);
	
	UFUNCTION()
	void LocalCardEvent(const FOrderUpdateEvent& Event);

	UFUNCTION()
	void LocalPlayerTakeDamage(const FOrderUpdateEvent& Event);
	
	//特殊事件，重分配手牌位置
	UFUNCTION(BlueprintCallable)
	void DecideHandLocation(ACardModel* Model);

	UFUNCTION(BlueprintCallable)
	void ModifyHandLocation(ACardModel* Model);

	//实例数组
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<ACardModel*> AllCards;
};
