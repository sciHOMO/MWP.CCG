#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact.generated.h"

UINTERFACE(BlueprintType, Blueprintable)
class MIRRORREALMPROTOCOL_API UInteract : public UInterface
{
	GENERATED_BODY()
};

class MIRRORREALMPROTOCOL_API IInteract
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartOverlap();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StopOverlap();
	
	UFUNCTION(BlueprintNativeEvent)
	void StartBeSelected();
	
	UFUNCTION(BlueprintNativeEvent)
	void StopBeSelected();
};