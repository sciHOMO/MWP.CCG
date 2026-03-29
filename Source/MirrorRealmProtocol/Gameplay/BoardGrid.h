#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MirrorRealmProtocol/Interface/Interact.h"
#include "BoardGrid.generated.h"

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API ABoardGrid : public AActor, public IInteract
{
	GENERATED_BODY()
	
public:	
	ABoardGrid();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntPoint Index = FIntPoint::ZeroValue;

};
