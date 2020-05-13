// Fill out your copyright notice in the Description page of Project Settings.

#include "VRHand.h"
#include "GBrush_Cpp.h"
#include "Pickupable.h"
#include "Ribbon.h"
#include "Runtime/HeadMountedDisplay/Public/MotionControllerComponent.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Runtime/Engine/Classes/Components/SplineMeshComponent.h"
#include "SplinePointRegister.h"
#include <vector>
#include <string>
#include <iostream>
#include "Components/StaticMeshComponent.h"



// Sets default values

AVRHand::AVRHand()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Hand = EControllerHand::Right;
	Grip = EGripState::Open;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	static ConstructorHelpers::FObjectFinder<UMaterial> Found(TEXT("/Game/Models/HTC_Vive_controllerMat.HTC_Vive_controllerMat"));
	if (Found.Succeeded()) {
		FoundMat = Found.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> PaintMat(TEXT("/Game/Assets/PaintingMaterial.PaintingMaterial"));
	if (PaintMat.Succeeded()) {
		Coloring = PaintMat.Object;
	}

	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Motion Controller"));
	MotionController->SetupAttachment(Scene);
	MotionController->Hand = Hand;

	HandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandMesh"));
	HandMesh->SetupAttachment(MotionController);

	GrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("GrabSph"));
	GrabSphere->SetupAttachment(HandMesh);
	GrabSphere->InitSphereRadius(10.0f);
	//OverLap event to Add GrabSphere->

	//DrawSphere->InitSphereRadius(5.0f);

	ArcDir = CreateDefaultSubobject<UArrowComponent>(TEXT("ArcDir"));
	ArcDir -> SetupAttachment(HandMesh);

	ArcSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ArcSpline"));
	ArcSpline->SetupAttachment(HandMesh);

	DrawSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DrawSphere"));
	DrawSphere->SetupAttachment(HandMesh);

	SplineOrbit = CreateDefaultSubobject<USplineComponent>(TEXT("SplineOrbit"));
	SplineOrbit->SetupAttachment(HandMesh);

	ArcEndPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcEndPoint"));
	ArcEndPoint->SetupAttachment(Scene);
	ArcEndPoint->SetVisibility(false);

	TeleportCylinder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportCyl"));
	TeleportCylinder->SetupAttachment(Scene);

	TeleportRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportRing"));
	TeleportRing->SetupAttachment(TeleportCylinder);

	TeleportArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TeleportArrow"));
	TeleportArrow->SetupAttachment(TeleportRing);

	RoomScaleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RoomScale"));
	RoomScaleMesh->SetupAttachment(TeleportRing);
}

void AVRHand::OnConstruction(const FTransform &Transform) {

	Super::OnConstruction(Transform);
	if (Hand == EControllerHand::Left) {

		HandMesh -> SetWorldScale3D(FVector(1, 1, -1));
	}
}

AActor* AVRHand::GetActorNearHand()
{
	TArray<AActor*> overlappingActors;

	GrabSphere->GetOverlappingActors(overlappingActors);
	FVector handLocation = GrabSphere->GetComponentLocation();

	AActor* nearest = nullptr;
	float mindist = 99999999999;

	// Find closest overlaping actor
	for (AActor *actor : overlappingActors)
	{
		bool isPickupable = actor->GetClass()->ImplementsInterface(UPickupable::StaticClass());
		if (isPickupable)
		{

			float dist = (actor->GetActorLocation() - handLocation).SizeSquared();
			if (dist < mindist)
			{
				mindist = dist;
				nearest = actor;
			}
		}
	}

	// 	if ( GEngine && Hand == EControllerHand::Right )
	// 		GEngine->AddOnScreenDebugMessage( -1, 0.16f, FColor::Red,
	// 			FString::Printf( TEXT( "Actors near right hand %d, found pickupable: %d, %s" ), 
	// 			overlappingActors.Num(), 
	// 			count,
	// 			nearest ? TEXT( "TRUE" ) : TEXT( "FALSE" ) ) );

	return nearest;
}

//IN THIS I NEED THE SPLINE DRAW e.t.c. CODE......
void AVRHand::DrawTheSpline(bool openGate, int &save, std::vector<SplinePointRegister> &splinePar) {
	FVector locationToSave;
	FVector tangentToSave;
	FVector InitializedLocation = DrawSphere->GetComponentLocation();
	SplinePointRegister justAPoint = SplinePointRegister();
	//HERE I KEEP EACH SWIPE'S POINT DATA
	if (openGate) {

		NewVar->SetActorLocation(InitializedLocation, false, false);

		SplineOrbit->AddSplinePoint(InitializedLocation, ESplineCoordinateSpace::World, true);


		SplineOrbit->GetLocationAndTangentAtSplinePoint(lastPoint, locationToSave, tangentToSave, ESplineCoordinateSpace::World);
		lastPoint++;
		justAPoint.assignSplinePoint(locationToSave, tangentToSave);
		pointsArray.push_back(justAPoint);  //ADDS EACH BRUSHES SWIPE POINT

		//SOME DEBUG SHIT
		int sizer = pointsArray.size();
		FString NewString = FString::FromInt(sizer) + " lalala";
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(12, 3, FColor::Green, NewString);
		}
		splinePar = pointsArray;
	}
	else if (!openGate && save==1) {
		//HERE I RESET THE pointsArray AND INSERT IT TO THE ORBIT OBJECT	
		masterOrbit.addOrbitToVector(pointsArray);
		save = 0;
		pointsArray.empty(); //RETURNS BOOL IF EMPTY
		pointsArray.clear(); //CLEARS THE ARRAY
		lastPoint = 0; //******may not need that*****

		//SOME DEBUG SHIT
		int sizer = masterOrbit.getOrbitsPointsSet().size();
		FString NewString = FString::FromInt(sizer);
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(12, 13, FColor::Black, NewString);
		}
		return;
	}

}

//IN THIS I NEED ONLY THE PAINTING CODE.....
void AVRHand::AirDrawing_Implementation(FVector &WorldLocation, bool isClicked) {

	FVector Location = DrawSphere->GetComponentLocation();
	FVector FwdVector = DrawSphere->GetForwardVector();
	FVector InitializedLocation = DrawSphere->GetComponentLocation();
	FVector locationToSave;
	FVector tangentToSave;

	WorldLocation = Location;

	FwdVector *= 155.0;
	Location += FwdVector;
	FDetachmentTransformRules detach(EDetachmentRule::KeepRelative, false);

	SplinePointRegister justAPoint = SplinePointRegister();
	if (!isClicked) {
		SplineOrbit->DetachFromComponent(detach);
		SplineOrbit->ClearSplinePoints(true);
		return;
	}
	else if (isClicked) {
		
		FActorSpawnParameters params;
		NewVar = GetWorld()->SpawnActor<ARibbon>(Location, FRotator(0, 0, 0));
		NewVar->ribbon->ActivateSystem(); //SEE LATER
		//**************NOTE TO MYSELF=Append NewVar Into Array Of Actors in Case I want CTRL-Z Function
		NewVar->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//HERE TO ADD SOME CODE....
		
		//HERE IS THE RIBBON PAINT MATERIAL THING, CAN'T USE THIS NOW
		PaintMaterial = UMaterialInstanceDynamic::Create(Coloring, this);
		PaintMaterial->SetVectorParameterValue("Coloring", ColorRGBA);
		NewVar->ribbon->SetMaterial(0, PaintMaterial);

		//NewVar->SetActorLocation(InitializedLocation, false, false);
		
		/*SplineOrbit->AddSplinePoint(InitializedLocation, ESplineCoordinateSpace::World, true);
		lastPoint++;
		
		SplineOrbit->GetLocationAndTangentAtSplinePoint(lastPoint, locationToSave, tangentToSave, ESplineCoordinateSpace::World);
		justAPoint.assignSplinePoint(locationToSave, tangentToSave);
		pointsArray.push_back(justAPoint);*/
	}
}

void AVRHand::RumbleController_Implementation(float Intensity) {
	FLatentActionInfo actionInfo;
	actionInfo.CallbackTarget = this;
	APlayerController *playerController = GetWorld()->GetFirstPlayerController();
	playerController->PlayHapticEffect(HapticEffect, Hand, Intensity, false);
}


// Called when the game starts or when spawned
void AVRHand::BeginPlay()
{
	Super::BeginPlay();
	if (Hand == EControllerHand::Right) {
		masterOrbit = OrbitRegister();
	}
	MotionController->Hand = Hand;
	if (Hand == EControllerHand::Left) {
		MotionController->Hand = Hand;
		HandMesh->SetWorldScale3D(FVector(1, 1, -1));
	}

	TeleportCylinder->SetVisibility(false, true);
}

OrbitRegister AVRHand::getMasterOrbit() {
	return masterOrbit;
}

void AVRHand::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Grip = EGripState::CanGrab;
	if ((OtherComp != nullptr) && (OtherComp != GrabSphere)) {

		UStaticMeshComponent *mesh = Cast<UStaticMeshComponent>(OtherComp);
		if (mesh && mesh->IsSimulatingPhysics()) {
			RumbleController(0.7);
		}

	}

}

void AVRHand::GrabActor_Implementation() {

	WantsToGrip = true;

	AActor *actor = GetActorNearHand();
	if (actor && actor->IsValidLowLevel()) {
		AttachedActor = actor;
		//IPickable::Execute_Pickup(actor, MotionController);
		RumbleController(0.7);
	}

}

void AVRHand::ReleaseActor_Implementation() {
	WantsToGrip = false;
	AActor *actor = AttachedActor;
	if (actor && actor->IsValidLowLevel()) {
		if (actor->GetRootComponent()->GetAttachParent()==MotionController) {
			//IPickable::Execute_Drop(actor, MotionController);
			RumbleController(0.2);
		}
	}

	AttachedActor = nullptr;
}

void AVRHand::ActivateTeleporter() {
	IsTeleporterActive = true;
	TeleportCylinder->SetVisibility(true, true);
	RoomScaleMesh->SetVisibility(IsRoomScale);
	if (MotionController) {
		InitialControllerRotation = MotionController->GetComponentRotation();
	}
}

void AVRHand::DisableTeleporter() {
	if (IsTeleporterActive) {
		IsTeleporterActive = false;
		TeleportCylinder->SetVisibility(false, true);
		ArcEndPoint->SetVisibility(false, false);
		for (USplineMeshComponent *SplineMesh : SplineMeshes)
			SplineMesh->SetVisibility(false);

		RoomScaleMesh->SetVisibility(false);
	}

}

void AVRHand::GetTeleporterDestination(FVector &OutLocation, FRotator &OutRotation) {
	IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
	FVector CylinderLocation = TeleportCylinder->GetComponentLocation();
	FVector DevicePosition(ForceInitToZero);
	if (hmd) {
		FRotator DeviceRotation;
		UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);
		DevicePosition.Z = 0;
	}
	DevicePosition = TeleportRotator.RotateVector(DevicePosition);
	OutRotation = TeleportRotator;
	OutLocation = CylinderLocation - DevicePosition;
}

bool AVRHand::TraceTeleportDestination(TArray<FVector> &TracePoints, FVector &NavMeshLocation, FVector &TraceLocation) {

	FVector ArcLocation = ArcDir->GetComponentLocation();
	FVector ArcForward = ArcDir->GetForwardVector();
	ArcForward *= TeleportLaunchVelocity;

	//predict projectile path
	FPredictProjectilePathParams PredictParams(0.0f, ArcLocation, ArcForward, 4.0f, UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	FPredictProjectilePathResult PredictResult;
	const bool DidPredictPath = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
	if (!DidPredictPath) {
		return false;
	}

	//predict Projected endpoint
	FVector PointToProject = PredictResult.HitResult.Location;
	FNavLocation ProjectedHitLocation;
	UNavigationSystem *NavSystem = GetWorld()->GetNavigationSystem();
	const bool PointToNavigation = NavSystem->ProjectPointToNavigation(PointToProject, ProjectedHitLocation, Extents);
	if (!PointToNavigation) {
		return false;
	}

	//outputs
	TracePoints.Empty();
	for (FPredictProjectilePathPointData Point : PredictResult.PathData)
		TracePoints.Push(Point.Location);

	NavMeshLocation = ProjectedHitLocation.Location;
	TraceLocation = PointToProject;
	return true;

}

FRotator AVRHand::GetControllerRelativeRotation() {
	const FTransform InitialTransform(InitialControllerRotation);
	const FTransform CurrentTransform = MotionController -> GetComponentTransform();
	const FTransform RelativeTransform = CurrentTransform.GetRelativeTransform(InitialTransform);

	return RelativeTransform.GetRotation().Rotator();
}



// Called every frame
void AVRHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Here I change the color of the viveController mesh
	HandMesh->SetVectorParameterValueOnMaterials(TEXT("None"), ColorRBGVector);
	//SOME DEBUG SHIT
	//if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(12, 3, FColor::Green, ColorRGBA.ToString());
	//}

	if (IsTeleporterActive) {
		TArray<FVector> TracePoints;
		FVector NavLocation;
		FVector HitLocation;
		bool IsValidTeleportDestination = TraceTeleportDestination(TracePoints, NavLocation, HitLocation);
		TeleportCylinder->SetVisibility(IsValidTeleportDestination, true);

		if (IsValidTeleportDestination) {

			//Floor Teleportation
			FHitResult OutHit;
			{
				FVector EndPos = NavLocation + FVector(0, 0, -200);
				FCollisionQueryParams CollisionQueryParams(FName(TEXT("Teleporter")), false, this);
				GetWorld()->LineTraceSingleByChannel(OutHit, NavLocation, EndPos, ECC_WorldStatic, CollisionQueryParams);
			}
			FVector TeleportCylinderLocation;
			if(OutHit.bBlockingHit) {
				TeleportCylinderLocation = OutHit.ImpactPoint;
			}
			else {
				TeleportCylinderLocation = NavLocation;
			}

			TeleportCylinder->SetWorldLocation(TeleportCylinderLocation, false, nullptr, ETeleportType::TeleportPhysics);

			ArcEndPoint->SetVisibility(true);
			ArcEndPoint->SetWorldLocation(HitLocation, false, nullptr, ETeleportType::TeleportPhysics);

			//Rotate Arrow
			FRotator ArrowRotator = TeleportRotator;
			IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();

			if (hmd) {
				FRotator DeviceRotation;
				FVector DevicePosition;
				UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(DeviceRotation, DevicePosition);

				DeviceRotation.Pitch = 0;
				DeviceRotation.Roll = 0;

				ArrowRotator = UKismetMathLibrary::ComposeRotators(TeleportRotator, DeviceRotation);
			}
			TeleportArrow->SetWorldRotation(ArrowRotator);

			//Spline Update
			if (TracePoints.Num() > 0) {
				ArcSpline->ClearSplinePoints();
				for (FVector TracePoint : TracePoints) {
					ArcSpline->AddSplinePoint(TracePoint, ESplineCoordinateSpace::Local, true);
				}
				ArcSpline->SetSplinePointType(TracePoints.Num() - 1, ESplinePointType::CurveClamped, true);

				for (int i = 0; i < TracePoints.Num(); i++) {
					FVector StartPos = TracePoints[i];
					FVector StartTangent = ArcSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);

					FVector EndPos = TracePoints[i + 1];
					FVector EndTangent = ArcSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

					USplineMeshComponent *SplineMeshCom;
					if (i >= SplineMeshes.Num()) {
						SplineMeshCom = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
						SplineMeshCom->SetStaticMesh(BeamMesh);
						SplineMeshCom->SetMaterial(0, BeamMaterial);
						SplineMeshCom->SetStartScale(FVector2D(4, 4));
						SplineMeshCom->SetEndScale(FVector2D(4, 4));
						SplineMeshCom->SetBoundaryMax(1);
						SplineMeshes.Push(SplineMeshCom);
					}
					SplineMeshCom = SplineMeshes[i];

					SplineMeshCom->SetVisibility(true);
					SplineMeshCom->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
				}
				for (int i = TracePoints.Num() - 2; i < SplineMeshes.Num(); i++) {
					SplineMeshes[i]->SetVisibility(false);
				}
				RegisterAllComponents();
			}


		}
		if (LastIsValidTeleportDestination != IsValidTeleportDestination) {
			RumbleController(0.4);
		}
		LastIsValidTeleportDestination = IsValidTeleportDestination;


	}
}

