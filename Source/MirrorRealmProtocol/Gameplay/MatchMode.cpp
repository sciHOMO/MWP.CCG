#include "MirrorRealmProtocol/Gameplay/MatchMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void AMatchMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (APlayerState* PlayerState = NewPlayer -> GetPlayerState<APlayerState>())
	{
		PlayerState -> SetPlayerId(GetGameState<AGameStateBase>() -> PlayerArray.Num() - 1);
	}
}
