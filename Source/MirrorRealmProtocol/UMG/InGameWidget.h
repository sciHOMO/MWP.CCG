#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MirrorRealmProtocol/Interface/ToUMG.h"
#include "InGameWidget.generated.h"

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API UInGameWidget : public UUserWidget, public IToUMG
{
	GENERATED_BODY()
	
	virtual void TurnChange_Implementation(const int PlayerID) override;
	virtual void ShowWinner_Implementation(const int PlayerID) override;
	virtual void ShowFulfillMessage_Implementation(const FCardEffectTarget& CardEffectTargets) override;
};
