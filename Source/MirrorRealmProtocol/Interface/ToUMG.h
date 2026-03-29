#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ToUMG.generated.h"

struct FCardEffectTarget;

UINTERFACE(BlueprintType, Blueprintable)
class MIRRORREALMPROTOCOL_API UToUMG : public UInterface
{
	GENERATED_BODY()
};

class MIRRORREALMPROTOCOL_API IToUMG
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent)
	void TurnChange(const int PlayerID);
	
	UFUNCTION(BlueprintNativeEvent)
	void ShowWinner(const int PlayerID);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowMontage(const int CardInstID);

	UFUNCTION(BlueprintNativeEvent)
	void ShowFulfillMessage(const FCardEffectTarget& CardEffectTargets);
};
