#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MiscFunction.generated.h"

enum class ECardSpec : uint8;

UCLASS()
class MIRRORREALMPROTOCOL_API UMiscFunction : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void ConvertMouseLocationToWorldLocation(APlayerController* PC, FVector& CursorLocation, FVector& ActorLocation, AActor*& Actor);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector TrackMouseLocationAtHeight(APlayerController* PC, const float Height);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FVector TrackSnapLocationAtHeight(APlayerController* PC, const float Height);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsInBoundary();
};
