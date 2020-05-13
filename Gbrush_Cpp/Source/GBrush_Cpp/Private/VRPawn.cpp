// Fill out your copyright notice in the Description page of Project Settings.

#include "VRPawn.h"
#include "GBrush_Cpp.h"
#include "VRHand.h"
#include "Ribbon.h"
#include "HandUIAttachment.h"
#include "SplinePointRegister.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "SculptingMeshes/SphereSculpting.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/HeadMountedDisplay/Public/IHeadMountedDisplay.h"
#include "SculptingMeshes/SphereSculpting.h"



// Sets default values
AVRPawn::AVRPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);

	LeftHand = CreateDefaultSubobject<UChildActorComponent>(TEXT("LeftHand"));
	LeftHand->SetupAttachment(VROrigin);

	RightHand = CreateDefaultSubobject<UChildActorComponent>(TEXT("RightHand"));
	RightHand->SetupAttachment(VROrigin);

	WidgetPointer = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetPointer"));
	WidgetPointer->SetupAttachment(RightHand);
	//WidgetPointer->SetAutoActive(true);

	UIHandBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UIHandBase"));
	UIHandBase->SetupAttachment(LeftHand);

	LeftHandUISculptMeshes = CreateDefaultSubobject<UChildActorComponent>(TEXT("UILeftHandSculptMesh"));
	LeftHandUISculptMeshes->SetupAttachment(UIHandBase);

	LeftHandUITexture = CreateDefaultSubobject<UChildActorComponent>(TEXT("UILeftHandTexture"));
	LeftHandUITexture->SetupAttachment(UIHandBase);

	LeftHandUI = CreateDefaultSubobject<UChildActorComponent>(TEXT("UILeftHand"));
	LeftHandUI->SetupAttachment(UIHandBase);

	ConstructorHelpers::FObjectFinder<UParticleSystem> PS(TEXT("/Game/Assets/IndexPointer.IndexPointer"));
	if (PS.Succeeded()) {
		SimplePointer = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SimplePointer"));
		SimplePointer->SetTemplate(PS.Object);
		SimplePointer->InitializeSystem();
		SimplePointer->SetupAttachment(RightHand);
	}

	ConstructorHelpers::FObjectFinder<UParticleSystem> PS2(TEXT("/Game/Assets/IndexPointerMesh.IndexPointerMesh"));
	if (PS2.Succeeded()) {
		SpawnMeshPointer = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MeshPointer"));
		SpawnMeshPointer->SetTemplate(PS2.Object);
		SpawnMeshPointer->InitializeSystem();
		SpawnMeshPointer->SetupAttachment(RightHand);
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> wallP(TEXT("/Game/Assets/decal_M.decal_M"));
	if (wallP.Succeeded()) {
		wallPaintMat = wallP.Object;
	}

	DeviceType = EHMDDeviceType::DT_ES2GenericStereoMesh;
	LocationPoint = FVector(0.f, 0.f, 0.f);
	tempPoints.empty();
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();
	SpawningMode = false;
	SpawningModePointer = false;
	spawnMeshType = 0;
	SculptingTool = false;
	IsDrawing = false;
	IsMouseClickedForColor = false;
	IsButtonUp = 0;
	IHeadMountedDisplay *hmd = GEngine->HMDDevice.Get();
	graffitiMode = false;
	RightBrush = Cast<AVRHand>(RightHand->GetChildActor());
	SpawningMode = false;
	SpawningModePointer = false;
	spawnMeshType = 0;
	SculptingTool = false;
	if (hmd) {
		DeviceType = hmd->GetHMDDeviceType();
		/*
		if (ControlScheme == ETeleportControlScheme::Auto)
		ControlScheme =
		(DeviceType == EHMDDeviceType::DT_OculusRift	? ETeleportControlScheme::StickOnly :
		(DeviceType == EHMDDeviceType::DT_Morpheus		? ETeleportControlScheme::ControllerRoll
		: ETeleportControlScheme::ButtonAndStick));*/


		if (DeviceType == EHMDDeviceType::DT_SteamVR || DeviceType == EHMDDeviceType::DT_OculusRift) {
			hmd->SetTrackingOrigin(EHMDTrackingOrigin::Floor);
		}

	}
	WidgetPointer->bShowDebug = false;
}

void AVRPawn::HandleButtonStyleTeleportActivation(UChildActorComponent *hand, EInputEvent KeyEvent) {
	if (ControlScheme == ETeleportControlScheme::StickOnly) {
		return;
	}

	AVRHand *Current = Cast<AVRHand>(hand->GetChildActor());
	AVRHand *Other = Cast<AVRHand>((hand == LeftHand ? RightHand : LeftHand)->GetChildActor());

	if (KeyEvent == IE_Pressed)
	{
		if (Current)
			Current->ActivateTeleporter();
		if (Other)
			Other->DisableTeleporter();
	}
	else
	{
		if (Current && Current->IsTeleporterActive)
			ExecuteTeleport(Current);
	}

}

/*
void AVRPawn::HandleStickInputStyleTeleportActivation(FVector2D AxisInput, AVRHand *Current, AVRHand *Other)
{
if (ControlScheme != ETeleportControlScheme::StickOnly)
return;

const float ThumbDeadzoneSq = ThumbDeadzone * ThumbDeadzone;
if (AxisInput.SizeSquared() > ThumbDeadzoneSq)
{
if (Current)
Current->ActivateTeleporter();
if (Other)
Other->DisableTeleporter();
}
else
{
if (Current && Current->IsTeleporterActive)
ExecuteTeleport(Current);
}
}*/



void AVRPawn::HandleGrip(UChildActorComponent *hand, EInputEvent KeyEvent) {
	AVRHand *current = Cast<AVRHand>(hand->GetChildActor());
	if (current) {
		if (KeyEvent == IE_Pressed) {
			current->GrabActor();
		}
		else {
			current->ReleaseActor();
		}
	}
}


bool AVRPawn::GetRotationFromInput(AVRHand *Current, FVector2D AxisInput, FRotator &OrientRotator)
{
	FRotator ActorRotator = GetActorRotation();
	ActorRotator.Roll = 0;
	ActorRotator.Pitch = 0;

	if (ControlScheme == ETeleportControlScheme::ControllerRoll)
	{
		const FRotator RelativeRotation = Current->GetControllerRelativeRotation();
		float FinalYaw = RelativeRotation.Roll * 3 + ActorRotator.Yaw;

		OrientRotator = FRotator(0, FinalYaw, 0);
		return true;
	}
	else
	{
		const float ThumbDeadzoneSq = ThumbDeadzone * ThumbDeadzone;
		if (AxisInput.SizeSquared() > ThumbDeadzoneSq)
		{
			FVector InputVector(AxisInput, 0);
			InputVector.Normalize();
			InputVector = ActorRotator.RotateVector(InputVector);

			OrientRotator = InputVector.ToOrientationRotator();
			return true;
		}
	}

	OrientRotator = ActorRotator;
	return false;
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InputComponent) {

		AVRHand *Left = Cast<AVRHand>(LeftHand->GetChildActor());
		FVector2D ThumbLeft(
			InputComponent->GetAxisValue(TEXT("ThumbLeft_Fwd")),
			InputComponent->GetAxisValue(TEXT("ThumbLeft_Side"))
			);

		AVRHand *Right = Cast<AVRHand>(RightHand->GetChildActor());
		FVector2D ThumbRight(
			InputComponent->GetAxisValue(TEXT("ThumbRight_Fwd")),
			InputComponent->GetAxisValue(TEXT("ThumbRight_Side"))
			);

		// Robo Rally style Teleport Activation
		//HandleStickInputStyleTeleportActivation(ThumbLeft, Left, Right);
		//HandleStickInputStyleTeleportActivation(ThumbRight, Right, Left);

		RightBrush->DrawTheSpline(GateOpened, IsButtonUp, tempPoints);

		if (TurnLeft) {
			UIHandBase->AddRelativeRotation(FRotator(0, 0, 120), false, nullptr, ETeleportType::None);
			FTransform temp = Left->GetTransform();
			FRotator rotateTemp = temp.Rotator();
			UIHandBase->SetWorldRotation(rotateTemp, false, nullptr, ETeleportType::None);
			TurnLeft = false;
		}
		if (TurnRight) {
			UIHandBase->AddRelativeRotation(FRotator(0, 0, -120), false, nullptr, ETeleportType::None);
			FTransform temp = Left->GetTransform();
			FRotator rotateTemp = temp.Rotator();
			UIHandBase->SetWorldRotation(rotateTemp, false, nullptr, ETeleportType::None);
			TurnRight = false;
		}


		int s = tempPoints.size();
		FString ss = FString::FromInt(s);
		//FVector LocationPoint = FVector(0.f, 0.f, 0.f);

		//Right->AirDrawing(LocationPoint, IsDrawing);

		// Get Teleport Target Rotation
		if (Left->IsTeleporterActive)
		{
			// If there is no Rotational Input, only use Actor rotation if not using Robo Rally style teleport
			FRotator OrientRotator;
			if (GetRotationFromInput(Left, ThumbLeft, OrientRotator) || ControlScheme != ETeleportControlScheme::StickOnly)
				Left->TeleportRotator = OrientRotator;
		}

		if (Right->IsTeleporterActive)
		{
			// If there is no Rotational Input, only use Actor rotation if not using Robo Rally style teleport
			FRotator OrientRotator;
			if (GetRotationFromInput(Right, ThumbRight, OrientRotator) || ControlScheme != ETeleportControlScheme::StickOnly)
				Right->TeleportRotator = OrientRotator;
		}

		/*HERE STARTS THE SCULPTING WHATEVER THING*/

		FVector startPoint = RightHand->GetComponentLocation();
		FVector endPoint = ((RightHand->GetForwardVector()) * 50) + startPoint;
		FHitResult OutHit;

		bool isHit = UKismetSystemLibrary::LineTraceSingle(RightHand, startPoint, endPoint, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::ForDuration, OutHit, false, FLinearColor::Red, FLinearColor::Blue, 1.5f);

		SimplePointer->SetBeamSourcePoint(0, OutHit.TraceStart, 0);
		if (!isHit) {
			SimplePointer->SetBeamTargetPoint(0, OutHit.TraceEnd, 0);
		}
		else {
			SimplePointer->SetBeamTargetPoint(0, OutHit.Location, 0);
		}

		SpawnMeshPointer->SetBeamSourcePoint(0, OutHit.TraceStart, 0);
		SpawnMeshPointer->SetBeamTargetPoint(0, OutHit.TraceEnd, 0);

		if (!SpawningModePointer) {
			SpawnMeshPointer->SetActive(false, false);
			SimplePointer->SetActive(true, false);
		}
		else if (SpawningModePointer) {
			SimplePointer->SetActive(false, false);
			SpawnMeshPointer->SetActive(true, false);
		}

		if (SpawningMode) {
			/*I must spawn actor here*/
			if (spawnMeshType == 1) {
				FRotator Rotation(0.0f, 0.0f, 0.0f);
				FActorSpawnParameters SpawnInfo;
				ASphereSculpting* tempSph = GetWorld()->SpawnActor<ASphereSculpting>(endPoint, Rotation, SpawnInfo);
				meshes.addSphere(tempSph);
			}
			SpawningMode = false;
		}

		if (OutHit.GetComponent() != NULL) {
			FString name = OutHit.GetComponent()->GetName();
			UKismetSystemLibrary::PrintString(this, name, true, false, FLinearColor::Blue, 1.0f);
		}

		if (graffitiMode) {
			//NOW RUN A FUNCTION
			//float forwardX = RightHand->GetForwardVector().X;
			float forwardX = Right->GetActorForwardVector().X;
			int x = UKismetMathLibrary::FTrunc(forwardX);
			float forwardXbyIncrement = x * 90;
			FRotator temp = Right->GetTransform().Rotator();
			FRotator rotateBy = FRotator(temp.Pitch, temp.Yaw, forwardXbyIncrement);
			paintingOnMeshes(Right, OutHit.Location, rotateBy);
		}


		TArray<UStaticMeshComponent*> spherePoints;
		if (SculptingTool == true)
		{
			ASphereSculpting *sphereToSculpt = Cast<ASphereSculpting>(OutHit.GetActor());
			ACubeSculpting *cubeToSculpt = Cast<ACubeSculpting>(OutHit.GetActor());
			AConeSculpting *coneToSculpt = Cast<AConeSculpting>(OutHit.GetActor());
			ACylinderSculpting *cylinderToSculpt = Cast<ACylinderSculpting>(OutHit.GetActor());
			if (sphereToSculpt != NULL) {
				spherePoints = sphereToSculpt->getProceduralMeshPoints();
			}
			else if (cubeToSculpt != NULL) {
				//spherePoints = cubeToSculpt->getProceduralMeshPoints();
			}
			else if (coneToSculpt != NULL) {
				//spherePoints = coneToSculpt->getProceduralMeshPoints();
			}
			else if (cylinderToSculpt != NULL) {
				//spherePoints = cylinderToSculpt->getProceduralMeshPoints();
			}
			FTransform newRelativeTransform;
			for (int32 i = 0; i < spherePoints.Num(); i++) {
				//FString *name = *FString(spherePoints.operator[](i).ToString());
				//UKismetSystemLibrary::PrintString(this, *FString(spherePoints.operator[](i)->GetName()), true, false, FLinearColor::Blue, 1.0f);

				//if (spherePoints.operator[](i)==OutHit.GetComponent()) {
				FVector newLocation = UKismetMathLibrary::InverseTransformDirection(spherePoints.operator[](i)->GetRelativeTransform(), DestVector);
				newRelativeTransform = UKismetMathLibrary::MakeTransform(newLocation, FRotator(0.0, 0.0, 0.0), FVector(0.1, 0.1, 0.1));
				if (UKismetMathLibrary::EqualEqual_ObjectObject(spherePoints.operator[](i), OutHit.GetComponent())) {
					UKismetSystemLibrary::PrintString(this, *FString(OutHit.GetComponent()->GetName()), true, true, FLinearColor::Red, 1.f);
					FVector newGivenTranslation;
					FRotator tmpRotation;
					FVector tmpScale;
					UKismetMathLibrary::BreakTransform(spherePoints.operator[](i)->GetRelativeTransform(), newGivenTranslation, tmpRotation, tmpScale);
					UKismetSystemLibrary::PrintString(this, *FString(newGivenTranslation.ToString()), true, true, FLinearColor::Red, 1.f);
					spherePoints.operator[](i)->SetVisibility(false, false);
					if (newGivenTranslation.X < 0.0) {
						DestX = DestLength + newGivenTranslation.X;
					}
					else if (newGivenTranslation.X > 0.0) {
						DestX = newGivenTranslation.X - DestLength;
					}
					if (newGivenTranslation.Y < 0.0) {
						DestY = DestLength + newGivenTranslation.Y;
					}
					else if (newGivenTranslation.Y > 0.0) {
						DestY = newGivenTranslation.Y - DestLength;
					}
					if (newGivenTranslation.Z < 0.0) {
						DestZ = DestLength + newGivenTranslation.Z;
					}
					else if (newGivenTranslation.Z > 0.0) {
						DestZ = newGivenTranslation.Z - DestLength;
					}
					DestVector.Set(DestX, DestY, DestZ);
					spherePoints.operator[](i)->SetRelativeTransform(newRelativeTransform, false, nullptr, ETeleportType::None);
					spherePoints.operator[](i)->SetVisibility(true, false);
					break;
				}
			}
			//spherePoints.operator[](0)->SetWorldLocation(startPoint, false, nullptr, ETeleportType::None);
		}



		//UWorld::AsyncLineTraceByChannel(startPoint,endPoint,ECollisionChannel::ECC_Camera,);

	}

}

void AVRPawn::setPaintingRGBA_Implementation(FLinearColor paintCol) {
	paintingRGBA = paintCol;
}

void AVRPawn::setBrushType_Implementation(int type) {
	brushType = type;
}

void AVRPawn::paintingOnMeshes(AVRHand *rightHand, FVector location, FRotator rotation) {
	ADecalActor* decal = GetWorld()->SpawnActor<ADecalActor>(location, rotation);
	if (decal) {
		UMaterialInstanceDynamic *ActorDecalSpawn = UMaterialInstanceDynamic::Create(wallPaintMat, this);
		decal->SetDecalMaterial(ActorDecalSpawn);
		decal->SetLifeSpan(0.0f);
		decal->GetDecal()->DecalSize = FVector(32.0f, 32.0f, 32.0f);
		ActorDecalSpawn->SetVectorParameterValue("paint", paintingRGBA);
		ActorDecalSpawn->SetScalarParameterValue("frame", brushType);
	}
}


void AVRPawn::ExecuteTeleport(AVRHand *Current)
{
	if (IsTeleporting)
		return;

	// 	if ( !Current->HasValidTeleportLocation )
	// 	{
	// 		Current->DisableTeleporter();
	// 		return;
	// 	}

	FVector TeleportPosition;
	FRotator TeleportRotator;

	Current->GetTeleporterDestination(TeleportPosition, TeleportRotator);

	// We're doing this!
	IsTeleporting = true;

	// Fade out screen
	APlayerCameraManager *PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	PlayerCamera->StartCameraFade(0, 1, FadeOutDuration, TeleportFadeColor, false, true);

	// Wait for Fade to complete before continuing the teleport
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName(TEXT("FinishTeleport")), Current, TeleportPosition, TeleportRotator);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, FadeOutDuration, false);
}


void AVRPawn::FinishTeleport(AVRHand *Current, const FVector &TeleportPosition, const FRotator &TeleportRotator)
{
	Current->DisableTeleporter();

	// Move the player
	TeleportTo(TeleportPosition, TeleportRotator, false, false);

	// Fade back in
	APlayerCameraManager *PlayerCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	PlayerCamera->StartCameraFade(1, 0, FadeInDuration, TeleportFadeColor, false, true);

	// All done.
	IsTeleporting = false;
}

void AVRPawn::BindInputActionUFunction(class UInputComponent* PlayerInputComponent, FName ActionName, EInputEvent KeyEvent, FName FuncName, UChildActorComponent *Hand)
{
	FInputActionBinding InputActionBinding(ActionName, KeyEvent);

	FInputActionHandlerSignature InputActionHandler;
	InputActionHandler.BindUFunction(this, FuncName, Hand, KeyEvent);

	InputActionBinding.ActionDelegate = InputActionHandler;
	PlayerInputComponent->AddActionBinding(InputActionBinding);
}

void AVRPawn::BindInputActionDrawingNSculpting(class UInputComponent* PlayerInputComponent, FName ActionName, EInputEvent KeyEvent, FName FuncName) {
	FInputActionBinding InputAction(ActionName, KeyEvent);

	FInputActionHandlerSignature InputActionHandler;
	InputActionHandler.BindUFunction(this, FuncName, KeyEvent);

	InputAction.ActionDelegate = InputActionHandler;
	PlayerInputComponent->AddActionBinding(InputAction);
}


// Called to bind functionality to input
//Here I add the spawn Bingings
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	BindInputActionUFunction(PlayerInputComponent, TEXT("TeleportLeft"), IE_Pressed, TEXT("HandleButtonStyleTeleportActivation"), LeftHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("TeleportLeft"), IE_Released, TEXT("HandleButtonStyleTeleportActivation"), LeftHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("TeleportRight"), IE_Pressed, TEXT("HandleButtonStyleTeleportActivation"), RightHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("TeleportRight"), IE_Released, TEXT("HandleButtonStyleTeleportActivation"), RightHand);

	BindInputActionUFunction(PlayerInputComponent, TEXT("GripLeft"), IE_Pressed, TEXT("HandleGrip"), LeftHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("GripLeft"), IE_Released, TEXT("HandleGrip"), LeftHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("GrabLeft"), IE_Pressed, TEXT("HandleGrip"), LeftHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("GrabLeft"), IE_Released, TEXT("HandleGrip"), LeftHand);

	BindInputActionUFunction(PlayerInputComponent, TEXT("GripRight"), IE_Pressed, TEXT("HandleGrip"), RightHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("GripRight"), IE_Released, TEXT("HandleGrip"), RightHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("GrabRight"), IE_Pressed, TEXT("HandleGrip"), RightHand);
	BindInputActionUFunction(PlayerInputComponent, TEXT("GrabRight"), IE_Released, TEXT("HandleGrip"), RightHand);

	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("Draw"), IE_Pressed, TEXT("StartDrawing"));
	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("Draw"), IE_Released, TEXT("StartDrawing"));

	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("SaveFile"), IE_Pressed, TEXT("saveFile"));
	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("SaveFile"), IE_Released, TEXT("saveFile"));

	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("ColorPicker"), IE_Pressed, TEXT("TranslateInput"));
	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("ColorPicker"), IE_Released, TEXT("TranslateInput"));

	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("SculptingTool"), IE_Pressed, TEXT("SculptingMode"));
	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("SculptingTool"), IE_Released, TEXT("SculptingMode"));

	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("SpawnMesh"), IE_Pressed, TEXT("SpawningMesh"));
	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("SpawnMesh"), IE_Released, TEXT("SpawningMesh"));

	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("Graffiti"), IE_Pressed, TEXT("enableWallPaint"));
	BindInputActionDrawingNSculpting(PlayerInputComponent, TEXT("Graffiti"), IE_Released, TEXT("enableWallPaint"));

	/*BindInputActionDrawing(PlayerInputComponent, TEXT("ColorPicker"), IE_Pressed, TEXT("TranslateInput"));
	BindInputActionDrawing(PlayerInputComponent, TEXT("ColorPicker"), IE_Pressed, TEXT("TranslateInput"));*/

	PlayerInputComponent->BindAxis(TEXT("ThumbLeft_Fwd"));
	PlayerInputComponent->BindAxis(TEXT("ThumbRight_Fwd"));

	PlayerInputComponent->BindAxis(TEXT("ThumbLeft_Side"));
	PlayerInputComponent->BindAxis(TEXT("ThumbRight_Side"));

}

//void AVRPawn::PrintSomething(EInputEvent KeyEvent) {
//	if (GEngine) {
//		GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Green, "I'm a debug message");
//	}
//	if (KeyEvent == IE_Pressed) {
//		if (GEngine) {
//			GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Green, "I'm not a debug message");
//		}
//	}
//}

void AVRPawn::enableWallPaint(EInputEvent KeyEvent) {
	if (KeyEvent == IE_Pressed) {
		graffitiMode = true;
	}
	else if (KeyEvent == IE_Released) {
		graffitiMode = false;
	}
}

void AVRPawn::SpawningMesh(EInputEvent KeyEvent) {
	if (KeyEvent == IE_Pressed) {
		SpawningModePointer = true;
		SpawningMode = false;
	}
	else if (KeyEvent == IE_Released) {
		SpawningMode = true;
		SpawningModePointer = false;
	}
}

void AVRPawn::SculptingMode(EInputEvent KeyEvent) {
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Green, "Sculpting Debug Msg");
	}
	if (KeyEvent == IE_Pressed) {
		SculptingTool = true;
	}
	else if (KeyEvent == IE_Released) {
		SculptingTool = false;
	}
}

void AVRPawn::TranslateInput(EInputEvent KeyEvent) {
	if (KeyEvent == IE_Pressed) {
		WidgetPointer->PressPointerKey(TEXT("LeftMouseButton"));
		IsMouseClickedForColor = true;
	}
	else if (KeyEvent == IE_Released) {
		WidgetPointer->ReleasePointerKey(TEXT("LeftMouseButton"));
		IsMouseClickedForColor = false;
	}
}

void AVRPawn::saveFile(EInputEvent KeyEvent) {
	if (KeyEvent == IE_Pressed) {
		//DEBUG SHIT AGAIN
		int temp = RightBrush->getMasterOrbit().printToFile();
		if (GEngine) {
			FString tt = FString::FromInt(temp);
			GEngine->AddOnScreenDebugMessage(12, 5.f, FColor::Green, tt);
		}
	}
}


void AVRPawn::StartDrawing_Implementation(EInputEvent KeyEvent) {
	AVRHand *Right = Cast<AVRHand>(RightHand->GetChildActor());
	//FVector LocationPoint = FVector(0.f, 0.f, 0.f);
	if (KeyEvent == IE_Pressed) {
		IsDrawing = true;
		GateOpened = true;
		IsButtonUp = 0;
		Right->AirDrawing(LocationPoint, IsDrawing);
	}
	else if (KeyEvent == IE_Released) {
		IsDrawing = false;
		GateOpened = false;
		IsButtonUp = 1;
		Right->AirDrawing(LocationPoint, IsDrawing);
	}
}
/*
void AVRPawn::AirDrawing_Implementation(class UChildActorComponent *hand) {
FVector Location = hand->GetComponentLocation();
FVector FwdVector = hand->GetForwardVector();
//FRotator Rotation = (0, 0, 0);

FwdVector *= 155.0;
Location += FwdVector;

//FTransform SpawnTransform = hand->transform
GetWorld()->SpawnActor<ARibbon>(Location, FRotator(0,0,0));

}*/