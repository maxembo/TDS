
#include "TPSPlayerController.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "../Character/TPSCharacter.h"
#include "Engine/World.h"
#include "GameFramework/HUD.h"
#include "Kismet/GameplayStatics.h"

ATPSPlayerController::ATPSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATPSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	/*if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}*/
	AimingUsingMouseCursor();
}


void ATPSPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ATPSPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ATPSPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ATPSPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ATPSPlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &ATPSPlayerController::OnResetVR);
}

void ATPSPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATPSPlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (ATPSCharacter* MyPawn = Cast<ATPSCharacter>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void ATPSPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ATPSPlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ATPSPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ATPSPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

bool ATPSPlayerController::GetMousePlanePosition(FVector& IntersectVector)
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	bool bHit = false;
	if(LocalPlayer && LocalPlayer->ViewportClient)
	{
		FVector2D MousePosition;
		if(LocalPlayer->ViewportClient->GetMousePosition(MousePosition))
		{
			bHit = GetMouseScreenPosition(MousePosition,IntersectVector);
		}
	}

	if(!bHit)
	{
		IntersectVector = FVector::ZeroVector;
	}
	return bHit;
}

bool ATPSPlayerController::GetMouseScreenPosition(FVector2D& ScreenPosition, FVector& IntersectVector)
{
	if(GetHUD() != NULL && GetHUD()->GetHitBoxAtCoordinates(ScreenPosition,true))
	{
		return false;
	}

	FVector WorldOrigin;
	FVector WorldDirection;
	if(UGameplayStatics::DeprojectScreenToWorld(this,ScreenPosition,WorldOrigin,WorldDirection) == true)
	{
		IntersectVector = FMath::LinePlaneIntersection(WorldOrigin,
														WorldOrigin + WorldDirection * HitResultTraceDistance,
													GetPawn()->GetActorLocation(),
													FVector::UpVector);
		return true;
	}
	return false;
}

void ATPSPlayerController::AimingUsingMouseCursor()
{
	if(!bMoveToMouseCursor)
	{
		return;
	}
	ATPSPlayerController* MyPawn = Cast<ATPSPlayerController>(GetPawn());
	if(MyPawn == nullptr)
	{
		return;
	}
	
	FVector PawnLoc = GetPawn()->GetActorLocation();
	FHitResult OutTraceResult;
	FVector IntersectVector;
	GetMousePlanePosition(IntersectVector);

	FCollisionQueryParams CollisionQueryParams("MouseAimingTrace",true);
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutTraceResult,IntersectVector,IntersectVector - FVector::UpVector * HitResultTraceDistance,ECC_Pawn,CollisionQueryParams);

	FVector Direction = FVector::ZeroVector;
	FVector Location = bHit ? OutTraceResult.ImpactPoint:IntersectVector;

	if(Location != FVector::ZeroVector)
	{
		Direction = Location -PawnLoc;

		DrawDebugLine(GetWorld(),PawnLoc,Location,FColor(255,255,0),false,-1,0,10.0f);
		if(bHit)
		{
			DrawDebugLine(GetWorld(),PawnLoc,Location,FColor(255,255,0),false,-1,0,10.0f);

		}
	}
}
