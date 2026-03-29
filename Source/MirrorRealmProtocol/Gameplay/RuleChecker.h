#pragma once

#include "CoreMinimal.h"
#include "RuleChecker.generated.h"

enum class ECardEffectType : uint8;
struct FCardInfo;
struct FCardEffect;
struct FQueueEffect;
class AMainServer;

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API URuleChecker : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="OMP_Interpreter")
	AMainServer* Server;

	//检查项
	UFUNCTION()
	bool PlayCardLegal(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index) const;

	UFUNCTION()
	bool PlayServantLegal(const int PlayerID,const int CardInstID, const int GridOwner, const FIntPoint Index) const;

	UFUNCTION()
	bool PlaySpellLegal(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index) const;

	UFUNCTION()
	bool AttackLegal(const int PlayerID, const int SourceCardInstID, const int TargetCardInstID) const;

	UFUNCTION()
	bool AttackDirectlyLegal(const int PlayerID, const int SourceCardInstID, const int TargetPlayerID) const;
	
	UFUNCTION()
	bool ActivateCardLegal(const int PlayerID, const int CardInstID);

	UFUNCTION()
	bool FindTargets(const FCardInfo& CardInfo, const FCardEffect& CardEffect) const;
	
	UFUNCTION()
	bool FindQueueTargets(const FCardInfo& CardInfo, const FQueueEffect& CardEffect) const;
};
