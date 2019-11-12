// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Components/WidgetInteractionComponent.h"
#include "Components/WidgetComponent.h"
#include "SplinePointRegister.h"
#include <vector>
#include "HandUIAttachment.h"
#include "MeshRegister.h"
#include "VRPawn.generated.h"


UENUM(BlueprintType)
enum class ETeleportControlScheme : uint8
{
	Auto,
	ButtonAndStick,
	StickOnly,
	ControllerRoll
};


UCLASS()
class GBRUSH_CPP_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent *VROrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent *Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent *LeftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent *RightHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent *LeftHandUI;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent *LeftHandUITexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent *LeftHandUISculptMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UWidgetInteractionComponent *WidgetPointer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *UIHandBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent *UIHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Components")
	class UParticleSystemComponent *SimplePointer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Components")
	class UParticleSystemComponent *SpawnMeshPointer;

	void HandleStickInputStyleTeleportActivation(FVector2D AxisInput, class AVRHand *Current, class AVRHand *Other);
	bool GetRotationFromInput(class AVRHand *Current, FVector2D AxisInput, FRotator &OrientRotator);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		float FadeInDuration = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		float FadeOutDuration = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		FLinearColor TeleportFadeColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		float ThumbDeadzone = 0.7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		float DefaultPlayerHeight = 180;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		bool SculptingTool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		bool IsMouseClickedForColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		bool TurnRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		bool SpawningMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		bool SpawningModePointer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		int spawnMeshType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		bool TurnLeft;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		ETeleportControlScheme ControlScheme = ETeleportControlScheme::Auto;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
		UMaterial *wallPaintMat;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//SOME SCULPTING VARIABLE SHIT
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables", meta = (AllowPrivateAccess = "true"))
		FVector DestVector = FVector(0.0, 0.0, 0.0);
	float DestX;
	float DestY;
	float DestZ;
	float DestLength = 1.5;
	//STOPS HERE

	MeshRegister meshes;


	uint8 DeviceType;
	bool IsTeleporting;
	bool IsDrawing;
	int IsButtonUp;

	std::vector<SplinePointRegister> tempPoints;
	FVector LocationPoint;
	bool GateOpened;
	AVRHand *RightBrush;

	// Sets default values for this pawn's properties
	AVRPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void HandleButtonStyleTeleportActivation(class UChildActorComponent *Hand, EInputEvent KeyEvent);

	UFUNCTION()
		void HandleGrip(class UChildActorComponent *Hand, EInputEvent KeyEvent);

	void BindInputActionUFunction(class UInputComponent* PlayerInputComponent, FName ActionName, EInputEvent KeyEvent, FName FuncName, class UChildActorComponent *Hand);

	void BindInputActionDrawingNSculpting(class UInputComponent* PlayerInputComponent, FName ActionName, EInputEvent KeyEvent, FName FuncName);

	UFUNCTION()
		void SculptingMode(EInputEvent KeyEvent);

	UFUNCTION()
		void TranslateInput(EInputEvent KeyEvent);

	UFUNCTION()
		void saveFile(EInputEvent KeyEvent);

	UFUNCTION()
		void SpawningMesh(EInputEvent KeyEvent);

	//UFUNCTION()
	//	void WidgetInput(EInputEvent KeyEvent);

	UFUNCTION()
		void FinishTeleport(class AVRHand *Current, const FVector &TeleportPosition, const FRotator &TeleportRotator);

	//UFUNCTION()
	//void PrintSomething(EInputEvent KeyEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void StartDrawing(EInputEvent KeyEvent);

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
	//void AirDrawing(class UChildActorComponent *hand);

	void ExecuteTeleport(class AVRHand *Current);
	void enableWallPaint(EInputEvent KeyEvent);
	void paintingOnMeshes(AVRHand *rightHand, FVector location, FRotator rotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void setPaintingRGBA(FLinearColor paintCol);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void setBrushType(int type);

private:
	UPROPERTY(EditAnywhere, Category="Code Variables")
		bool graffitiMode;

	UPROPERTY(EditAnyWhere, Category = "Code Variables")
		FLinearColor paintingRGBA;

	UPROPERTY(EditAnywhere, Category = "Code Variables")
		int brushType;

};
