#include "MirrorRealmProtocol/Gameplay/CardModel.h"
#include "MirrorRealmProtocol/Gameplay/BoardGrid.h"
#include "MirrorRealmProtocol/Gameplay/VisualManager.h"
#include "MirrorRealmProtocol/Gameplay/GameStruct.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

void ACardModel::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (CardState == ECardState::LinearLep)
	{
		const FVector CurrentLocation = GetActorLocation();
		const FVector CurrenScale = GetActorScale3D();
		const FVector NextLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaSeconds, 8.0f);
		const FVector NextScale = FMath::Lerp(CurrenScale, FVector::OneVector, 0.1f);
		SetActorLocation(NextLocation);
		SetActorScale3D(NextScale);
	}
	
	if (CardState == ECardState::QuickLep)
	{
		const FVector CurrentLocation = GetActorLocation();
		const FVector CurrenScale = GetActorScale3D();
		const FVector NextLocation = FMath::Lerp(CurrentLocation, TargetLocation, 0.1f);
		const FVector NextScale = FMath::Lerp(CurrenScale, FVector::OneVector, 0.1f);
		SetActorLocation(NextLocation);
		SetActorScale3D(NextScale);
	}

	if (CardState == ECardState::LinearLep || CardState == ECardState::QuickLep)
	{
		if (UKismetMathLibrary::Vector_IsNearlyZero(GetActorLocation() - TargetLocation, 0.1f))
		{
			CardState = ECardState::Static;
		}
	}
	
	AdditiveLocation =  CalculateAdditiveLocation();
	const FVector CardCurrentLocation = CardMesh -> GetRelativeLocation();
	const FVector CardNextLocation = FMath::Lerp(CardCurrentLocation, AdditiveLocation, 0.1f);
	CardMesh -> SetRelativeLocation(CardNextLocation);
	const FVector PawnCurrentLocation = PawnMesh -> GetRelativeLocation();
	const FVector PawnNextLocation = FMath::Lerp(PawnCurrentLocation, AdditiveLocation, 0.1f);
	PawnMesh -> SetRelativeLocation(PawnNextLocation);
	
	CardOutline -> SetVisibility((bIsOverlap || bIsSelected) && CardMesh -> IsVisible() && Team == ETeamType::Owner && CardState == ECardState::Static);
	PawnOutline -> SetVisibility((bIsOverlap || bIsSelected) && PawnMesh -> IsVisible() && Team == ETeamType::Owner && CardState == ECardState::Static);
}

void ACardModel::Destroyed()
{
	if (VisualManager)
	{
		VisualManager -> AllCards.Remove(this);
		VisualManager -> AllCards.Shrink();
	}
	Super::Destroyed();
}

ACardModel::ACardModel()
{
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));
	RootComponent = Root;
	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMesh"));
	CardMesh -> SetupAttachment(Root);
	CardMesh -> SetVisibility(true);
	CardOutline = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardOutline"));
	CardOutline -> SetupAttachment(CardMesh);
	CardOutline -> SetVisibility(false);
	CardCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CardCollision"));
	CardCollision -> SetupAttachment(Root);
	
	PawnMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	PawnMesh -> SetupAttachment(Root);
	PawnMesh -> SetVisibility(false);
	PawnOutline = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnOutline"));
	PawnOutline -> SetupAttachment(PawnMesh);
	PawnOutline -> SetVisibility(false);
	PawnCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PawnCollision"));
	PawnCollision -> SetupAttachment(Root);

}

void ACardModel::SetHandLocation()
{
	FVector Start, End;
	if (CardInfo.PlayerID == 0)
	{
		Start = FVector(-450.f, -300.f, 200.f);
		End = FVector(-450.f, 300.f, 250.f);
	}
	else
	{
		Start = FVector(450.f, 300.f, 200.f);
		End = FVector(450.f, -300.f, 250.f);
	}

	//掐头去尾（为了美观）
	const int AdjustedTotal = TotalInHand + 1;
	const int AdjustedIndex = IndexInHand + 1;
	
	const float Alpha = TotalInHand == 1 ? 0.5f : static_cast<float>(AdjustedIndex) / static_cast<float>(AdjustedTotal);
	TargetLocation = FMath::Lerp(Start, End, Alpha);
	CardState = ECardState::LinearLep;
}

void ACardModel::SetEntryTransform(const FIntPoint Index)
{
	ABoardGrid* Grid = nullptr;
	TArray<AActor*> AllGrids;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoardGrid::StaticClass(), AllGrids);
	for (AActor* Emt : AllGrids)
	{
		if (Emt && Cast<ABoardGrid>(Emt) -> Index == Index)
		{
			if ((CardInfo.PlayerID == 0 && Cast<ABoardGrid>(Emt) -> Tags.Contains("Player_0")) || (CardInfo.PlayerID == 1 && Cast<ABoardGrid>(Emt) -> Tags.Contains("Player_1")))
			{
				Grid = Cast<ABoardGrid>(Emt);
				break;
			}
		}
	}
	if (!Grid)	return;
	SetActorLocation(FVector(Grid -> GetActorLocation().X, Grid -> GetActorLocation().Y, 200.0f));
	SetActorScale3D(FVector::OneVector);
}

void ACardModel::PlayDrawAnim_Implementation(const FOrderUpdateEvent& Event)
{
	CardState = ECardState::Anim;
}

void ACardModel::DrawAnimEnd()
{
	CardState = ECardState::LinearLep;
	if(VisualManager)
	{
		VisualManager -> Continue();
	}
}

void ACardModel::PlayEntryAnim_Implementation(const FOrderUpdateEvent& Event)
{
	CardState = ECardState::Anim;
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("PlayEntryAnim_C"), Event);
	GetWorld() -> GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.55f, false);
}

void ACardModel::PlayEntryAnim_C_Implementation(const FOrderUpdateEvent& Event)
{
	SetEntryTransform(Event.PositionInfo[1]);
	CardMesh -> SetVisibility(false);
	PawnMesh -> SetVisibility(false);
}

void ACardModel::EntryAnimEnd()
{
	CardState = ECardState::LinearLep;
	TargetLocation = GetActorLocation();
	if(VisualManager)
	{
		VisualManager -> Continue();
	}
}

void ACardModel::PlayCastAnim_Implementation(const FOrderUpdateEvent& Event)
{
	CardState = ECardState::Anim;	
}

void ACardModel::CastAnimEnd()
{
	CardState = ECardState::LinearLep;
	if(VisualManager)
	{
		VisualManager -> Continue();
	}
	Destroy();
}

void ACardModel::PlayAttackAnim_Implementation(const FOrderUpdateEvent& Event)
{
	CardState = ECardState::Anim;
}

void ACardModel::AttackAnimEnd_Implementation()
{
	CardState = ECardState::LinearLep;
	if(VisualManager)
	{
		VisualManager -> Continue();
	}
}

void ACardModel::PlayDamageAnim_Implementation(const FOrderUpdateEvent& Event)
{
	CardState = ECardState::Anim;
}

void ACardModel::DamageAnimEnd()
{
	CardState = ECardState::LinearLep;
	if(IsPendingKill)
	{
		Destroy();
	}
	if(VisualManager)
	{
		VisualManager -> Continue();
	}
}

void ACardModel::PlayActivateAnim_Implementation(const FOrderUpdateEvent& Event)
{
	CardState = ECardState::Anim;
}

void ACardModel::ActivateAnimEnd()
{
	CardState = ECardState::LinearLep;
	if(VisualManager)
	{
		VisualManager -> Continue();
	}
}

void ACardModel::ReceiveCardInfo(const FCardInfo& CI)
{
	if(CardInfo != CI)
	{
		CardInfo = CI;
		UpdateCardInfo(CardInfo);
	}
}

void ACardModel::UpdateCardInfo_Implementation(const FCardInfo& CI)
{
	
}

void ACardModel::TryKill()
{
	if (CardState == ECardState::Anim)
	{
		IsPendingKill = true;
	}
	else
	{
		Destroy();
	}
}

FVector ACardModel::CalculateAdditiveLocation()
{
	if (CardState != ECardState::Static) return FVector::ZeroVector;
		
	if (!bIsOverlap && !bIsSelected) return FVector::ZeroVector;
	
	switch(Location)
	{
	case ELocation::Hand :
		{
			if (Team == ETeamType::Owner)
			{
				return FVector(-50.0f, 0.0f, 100.0f);
			}
			break;
		}
	case ELocation::Board :
		{
			if (Team == ETeamType::Owner)
			{
				return FVector(0.0f, 0.0f, 50.0f);
			}
			break;
		}	
	default : break;
	}
	return FVector::ZeroVector;
}

void ACardModel::StartOverlap_Implementation()
{
	bIsOverlap = true;
}

void ACardModel::StopOverlap_Implementation()
{
	bIsOverlap = false;
}

void ACardModel::StartBeSelected_Implementation()
{
	bIsSelected = true;
}

void ACardModel::StopBeSelected_Implementation()
{
	bIsSelected = false;
}
