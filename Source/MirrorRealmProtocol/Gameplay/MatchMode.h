#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MatchMode.generated.h"

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API AMatchMode : public AGameMode
{
	GENERATED_BODY()

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};
