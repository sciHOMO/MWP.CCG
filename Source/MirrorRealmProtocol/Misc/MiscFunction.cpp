#include "MirrorRealmProtocol/Misc/MiscFunction.h"

void UMiscFunction::ConvertMouseLocationToWorldLocation(APlayerController* PC, FVector& CursorLocation, FVector& ActorLocation, AActor*& Actor)
{
	FVector Direction = FVector::ZeroVector;
	PC -> DeprojectMousePositionToWorld(CursorLocation, Direction);

	FHitResult HitResult;
	if (PC -> GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult))
	{
		Actor = HitResult.GetActor();
		ActorLocation = Actor -> GetActorLocation();
	}
}

FVector UMiscFunction::TrackMouseLocationAtHeight(APlayerController* PC,const float Height)
{
	if (!PC) return FVector::ZeroVector;
	
	if (FVector WorldLocation, WorldDirection; PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		
		const float T = (Height - WorldLocation.Z) / WorldDirection.Z;
		if (T >= 0)
		{
			return WorldLocation + WorldDirection * T;
		}
	}
	return FVector::ZeroVector;
}

FVector UMiscFunction::TrackSnapLocationAtHeight(APlayerController* PC, const float Height)
{
	if (!PC) return FVector::ZeroVector;
	if (FHitResult  HitResult; PC->GetHitResultUnderCursor(ECC_Visibility, false,  HitResult))
	{
		 HitResult.Location.Z = Height;
		return  HitResult.Location;
	}
	return TrackMouseLocationAtHeight(PC, Height);
}

bool UMiscFunction::IsInBoundary()
{
	bool bIsBoundary = false;
	if (GEngine->GameViewport)
	{
		FVector2D MousePosition;
		FVector2D ScreenSize;
		GEngine -> GameViewport -> GetMousePosition(MousePosition);
		GEngine -> GameViewport -> GetViewportSize(ScreenSize);
		
		const float TopBoundary = ScreenSize.Y * 0.25f;
		const float BottomBoundary = ScreenSize.Y * 0.75f;
		const float LeftBoundary = ScreenSize.X * 0.25f;
		const float RightBoundary = ScreenSize.X * 0.75f;
		bIsBoundary = MousePosition.Y >= TopBoundary && MousePosition.Y <= BottomBoundary && MousePosition.X >= LeftBoundary && MousePosition.X <= RightBoundary;
	}
	return bIsBoundary;
};

