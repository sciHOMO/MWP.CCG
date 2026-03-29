#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LocalDecks.generated.h"

UCLASS(Blueprintable, BlueprintType)
class MIRRORREALMPROTOCOL_API ULocalDecks : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DeckIndex = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Deck_001 = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Deck_002 = {6, 7, 8, 9, 10, 6, 7, 8, 9, 10};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Deck_003;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Deck_004;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Deck_005;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Deck_006;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Deck_007;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> Deck_008;
	
	UFUNCTION()
	TArray<int> FindDeckByIndex(const int Index) const;
};
