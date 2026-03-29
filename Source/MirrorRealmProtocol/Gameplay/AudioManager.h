#pragma once

#include "CoreMinimal.h"
#include "AudioManager.generated.h"

struct FOrderUpdateEvent;
class AGameController;

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API UAudioManager : public UObject
{
	GENERATED_BODY()
	
public:	

	//初始化
	UFUNCTION()
	void BeginPlay();
	
	//接收事件
	UFUNCTION()
	void ReceiveEvent(FOrderUpdateEvent NewEvent);

	UFUNCTION()
	void HandleEvent();

	UFUNCTION()
	void Continue();

	//场景绑定
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGameController* Controller;
};
