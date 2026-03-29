#include "MirrorRealmProtocol/Gameplay/GameController.h"
#include "MirrorRealmProtocol/Gameplay/BoardGrid.h"
#include "MirrorRealmProtocol/Gameplay/CardModel.h"
#include "MirrorRealmProtocol/Gameplay/MainServer.h"
#include "MirrorRealmProtocol/Gameplay/VisualManager.h"
#include "MirrorRealmProtocol/Gameplay/AudioManager.h"
#include "MirrorRealmProtocol/Gameplay/LocalDecks.h"
#include "MirrorRealmProtocol/Misc/MiscFunction.h"
#include "MirrorRealmProtocol/Interface/ToUMG.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Engine/World.h"
#include "TimerManager.h"



class ULocalDecks;

void AGameController::BeginPlay()
{
	Super::BeginPlay();
	BothLoadComponent();
}

void AGameController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AGameController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	BothLoadComponent();
}

void AGameController::BothLoadComponent_Implementation()
{
	if (HasAuthority())
	{
		Server = CastChecked<AMainServer>( UGameplayStatics::GetGameState(GetWorld()));
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld() -> GetTimerManager().SetTimer(TimerHandle, this, &AGameController::LoadComponent, 5.0f);
	}
}

void AGameController::LoadComponent()
{
	if (!Server)
	{
		Server = CastChecked<AMainServer>( UGameplayStatics::GetGameState(GetWorld()));
	}
	if (!Server || !PlayerState)
	{
		GetWorld() -> GetTimerManager().SetTimerForNextTick(this, &AGameController::LoadComponent);
	}
	else
	{
		VisualManager = NewObject<UVisualManager>(this, VisualManagerClass);
		check(VisualManager);
		VisualManager -> Controller = this;
		VisualManager -> BeginPlay();

		AudioManager = NewObject<UAudioManager>(this, AudioManagerClass);
		check(AudioManager);
		AudioManager -> Controller = this;
		AudioManager -> BeginPlay();
		
		TArray<AActor*> Camera;
		const FName Tag = PlayerState -> GetPlayerId() == 0 ? TEXT("Player_0") : TEXT("Player_1");
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACameraActor::StaticClass(), Tag, Camera);
		SetViewTarget(Camera[0]);

		TrySendDeckToServer();
	}
}

void AGameController::OnFirstPress_Implementation()
{
	FVector CursorLocation;
	FVector ActorLocation;
	AActor* FoundActor = nullptr;
	UMiscFunction::ConvertMouseLocationToWorldLocation(this, CursorLocation, ActorLocation, FoundActor);
	if (FoundActor && FoundActor -> GetClass() -> ImplementsInterface(UInteract::StaticClass()) && Cast<ACardModel>(FoundActor))
	{
		FirstPressed =FoundActor;
		IInteract::Execute_StartBeSelected(FoundActor);
	}
}

void AGameController::OnFirstRelease_Implementation()
{
	FVector CursorLocation;
	FVector ActorLocation;
	AActor* FoundActor = nullptr;
	UMiscFunction::ConvertMouseLocationToWorldLocation(this, CursorLocation, ActorLocation, FoundActor);
	if (FirstPressed && FoundActor && FoundActor -> GetClass() -> ImplementsInterface(UInteract::StaticClass()) && Cast<ACardModel>(FoundActor) && Server -> GetCardInstCopybyID(Cast<ACardModel>(FirstPressed) -> CardInfo.CardInstID) -> CardType == ECardType::Servant)
	{
		FirstReleased = FoundActor;
		IInteract::Execute_StartBeSelected(FoundActor);
		if (FirstPressed != FirstReleased)
		{
			RequestAttack(PlayerState -> GetPlayerId(), Cast<ACardModel>(FirstPressed) ->  CardInfo.CardInstID, Cast<ACardModel>(FirstReleased) -> CardInfo.CardInstID);
		}
		else return;
	}
	if (FirstPressed && Server -> GetCardInstCopybyID(Cast<ACardModel>(FirstPressed) -> CardInfo.CardInstID) -> CardType == ECardType::Spell)
	{
		if (UMiscFunction::IsInBoundary())
		{
			RequestPlayCard(PlayerState -> GetPlayerId(), Cast<ACardModel>(FirstPressed) ->  CardInfo.CardInstID, INT_MAX, FIntPoint::ZeroValue);
		}
	}
	if (FirstPressed && FoundActor && FoundActor -> GetClass() -> ImplementsInterface(UInteract::StaticClass()) && Cast<ABoardGrid>(FoundActor))
	{
		FirstReleased = FoundActor;
		IInteract::Execute_StartBeSelected(FoundActor);
		if (Cast<ABoardGrid>(FirstReleased) -> Index == FIntPoint(-5, -5))
		{
			int GridOwner = INT_MAX;
			if (Cast<ABoardGrid>(FirstReleased) -> Tags.Contains("Player_0")) GridOwner = 0;
			if (Cast<ABoardGrid>(FirstReleased) -> Tags.Contains("Player_1")) GridOwner = 1;
			RequestAttackDirectly(PlayerState -> GetPlayerId(), Cast<ACardModel>(FirstPressed) ->  CardInfo.CardInstID , GridOwner);
		}
		else
		{
			int GridOwner = INT_MAX;
			if (Cast<ABoardGrid>(FirstReleased) -> Tags.Contains("Player_0")) GridOwner = 0;
			if (Cast<ABoardGrid>(FirstReleased) -> Tags.Contains("Player_1")) GridOwner = 1;
			RequestPlayCard(PlayerState -> GetPlayerId(), Cast<ACardModel>(FirstPressed) -> CardInfo.CardInstID, GridOwner, Cast<ABoardGrid>(FirstReleased) -> Index);
		}
	}

	if (FirstPressed && FirstPressed -> GetClass() -> ImplementsInterface(UInteract::StaticClass()))
	{
		IInteract::Execute_StopBeSelected(FirstPressed);
		FirstPressed = nullptr;
	}
	if (FirstReleased && FirstReleased-> GetClass() -> ImplementsInterface(UInteract::StaticClass()))
	{
		IInteract::Execute_StopBeSelected(FirstReleased);
		FirstReleased = nullptr;
	}
}

void AGameController::OnSecondPress_Implementation()
{
	FVector CursorLocation;
	FVector ActorLocation;
	AActor* FoundActor = nullptr;
	UMiscFunction::ConvertMouseLocationToWorldLocation(this, CursorLocation, ActorLocation, FoundActor);
	if (FoundActor && FoundActor -> GetClass() -> ImplementsInterface(UInteract::StaticClass()))
	{
		SecondPressed = FoundActor;
	}
}

void AGameController::OnSecondRelease_Implementation()
{
	FVector CursorLocation;
	FVector ActorLocation;
	AActor* FoundActor = nullptr;
	UMiscFunction::ConvertMouseLocationToWorldLocation(this, CursorLocation, ActorLocation, FoundActor);
	if (FoundActor && FoundActor -> GetClass() -> ImplementsInterface(UInteract::StaticClass()))
	{
		SecondReleased = FoundActor;
	}
	if (FirstPressed && FirstReleased && SecondPressed && SecondReleased && FirstPressed == FirstReleased && FirstReleased == SecondPressed &&  SecondPressed ==SecondReleased)
	{
		RequestActivateCard(PlayerState -> GetPlayerId(), Cast<ACardModel>(FirstPressed) ->  CardInfo.CardInstID);
		
		if (FirstPressed -> GetClass() -> ImplementsInterface(UInteract::StaticClass()))
		{
			IInteract::Execute_StopBeSelected(FirstPressed);
			FirstPressed = nullptr;
		}
		if (FirstReleased-> GetClass() -> ImplementsInterface(UInteract::StaticClass()))
		{
			IInteract::Execute_StopBeSelected(FirstReleased);
			FirstReleased = nullptr;
		}
		SecondPressed = nullptr;
		SecondReleased = nullptr;
	}
}

void AGameController::OnClickTarget_Implementation()
{
	FVector CursorLocation;
	FVector ActorLocation;
	AActor* FoundActor = nullptr;
	UMiscFunction::ConvertMouseLocationToWorldLocation(this, CursorLocation, ActorLocation, FoundActor);
	if (FoundActor)
	{
		CheckAndPass(FoundActor);
	}
}

void AGameController::RequestEndTurn_Implementation(const int PlayerID)
{
	if (Server)
	{
		Server -> RequestEndTurn(PlayerID);
	}
}

void AGameController::RequestPlayCard_Implementation(const int PlayerID, const int CardInstID, const int GridOwner, const FIntPoint Index)
{
	if (Server)
	{
		Server -> RequestPlayCard(PlayerID, CardInstID, GridOwner, Index);
	}
}

void AGameController::RequestAttack_Implementation(const int PlayerID, const int SourceCardInstID, const int TargetCardInstID)
{
	if (Server)
	{
		Server -> RequestAttack(PlayerID, SourceCardInstID, TargetCardInstID);
	}
}

void AGameController::RequestAttackDirectly_Implementation(const int PlayerID, const int SourceCardInstID, const int TargetPlayerID)
{
	if (Server)
	{
		Server -> RequestAttackDirectly(PlayerID, SourceCardInstID, TargetPlayerID);
	}
}

void AGameController::RequestActivateCard_Implementation(const int PlayerID, const int CardInstID)
{
	if (Server)
	{
		Server -> RequestActivateCard(PlayerID, CardInstID);
	}
}

void AGameController::RequestFulfillTargets_Implementation(const TArray<int>& TIDs)
{
	if (Server)
	{
		Server -> RequestFulfillTargets(TIDs);
	}
}

void AGameController::TrySendDeckToServer()
{
	if (HasAuthority()) return;
	if (const USaveGame* LocalDeck = UGameplayStatics::LoadGameFromSlot(TEXT("LocalDecks"), 0))
	{
		if (const ULocalDecks* FoundLocalDeck = Cast<ULocalDecks>(LocalDeck))
		{
			SendDeckToServer(PlayerState -> GetPlayerId(), FoundLocalDeck -> FindDeckByIndex(1));
		}
	}
}

void AGameController::SendDeckToServer_Implementation(const int PlayerID, const TArray<int>& CardPreInfo)
{
	if (!HasAuthority()) return;
	if (Server)
	{
		Server -> CopyDeckFromClient(PlayerID, CardPreInfo);
	}
}

void AGameController::CopyDeckFromServer_Implementation()
{
	if(HasAuthority()) return;
	if(Server && Server -> Decks.Num() == Server -> DeckSize * 2)
	{
		ClientControllerReady();
	}
}

void AGameController::ClientControllerReady_Implementation()
{
	if(!HasAuthority()) return;
	if(Server)
	{
		Server -> CheckClientsReady();
	}
}

void AGameController::ReceiveFulfillEvent(const TArray<FCardEffectTarget>& CET)
{
	CardEffectTargets = CET;
	CardEffectTargetIndex = 0;
	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld(), AllWidgets, UToUMG::StaticClass(), true);
	IToUMG::Execute_ShowFulfillMessage(AllWidgets[0],  CardEffectTargets[CardEffectTargetIndex]);
}

void AGameController::CheckAndPass(AActor* FoundActor)
{
	if (true)
	{
		TargetInstIDs.Emplace(Cast<ACardModel>(FoundActor) -> CardInfo.CardInstID);
		CardEffectTargetIndex ++;
		if (CardEffectTargets.Num() == TargetInstIDs.Num())
		{
			RequestFulfillTargets(TargetInstIDs);
			CardEffectTargets.Empty();
			CardEffectTargets.Shrink();
			TargetInstIDs.Empty();
			TargetInstIDs.Empty();
		}
		if (CardEffectTargets.IsValidIndex(CardEffectTargetIndex))
		{
			TArray<UUserWidget*> AllWidgets;
			UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld(), AllWidgets, UToUMG::StaticClass(), true);
			IToUMG::Execute_ShowFulfillMessage(AllWidgets[0], CardEffectTargets[CardEffectTargetIndex]);
		}
		else
		{
			TArray<UUserWidget*> AllWidgets;
			UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld(), AllWidgets, UToUMG::StaticClass(), true);
			IToUMG::Execute_ShowFulfillMessage(AllWidgets[0], FCardEffectTarget());
		}
	}
}






