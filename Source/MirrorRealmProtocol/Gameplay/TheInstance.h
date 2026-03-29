#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TheInstance.generated.h"

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API UTheInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;
	
	UFUNCTION(Category="OMP_Game")
	void GeneratePlayerProfile();
	
};
