// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "SplinePointRegister.h"
#include "OrbitRegister.h"
#include <vector>
#include "VRHand.generated.h"


UENUM(BlueprintType)
enum class EGripState : uint8 
{
	Open,
	CanGrab,
	Grab
};


UCLASS()
class GBRUSH_CPP_API AVRHand : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVRHand();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta=(AllowPrivateAccess="true"))
	class USceneComponent *Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent *MotionController;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *HandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent *DrawSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent *GrabSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class USplineComponent *SplineOrbit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UArrowComponent *ArcDir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class USplineComponent *ArcSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *ArcEndPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *TeleportCylinder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *TeleportRing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *TeleportArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *RoomScaleMesh;

	FRotator InitialControllerRotation;
	TArray<class USplineMeshComponent*> SplineMeshes;

public:
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
	EControllerHand Hand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
	EGripState Grip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
	class UStaticMesh *BeamMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
	class UMaterial *BeamMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
	class UHapticFeedbackEffect_Base *HapticEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
	float TeleportLaunchVelocity = 900;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Constants")
	FVector Extents;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
	bool WantsToGrip;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
	bool IsTeleporterActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
	class AActor *AttachedActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
	bool IsRoomScale;

	std::vector<SplinePointRegister> pointsArray;
	OrbitRegister getMasterOrbit();
	OrbitRegister masterOrbit;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		class ARibbon *NewVar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
	class UMaterialInstanceDynamic *PaintMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
		UMaterial *Coloring;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Code Variables")
		FVector ColorRBGVector;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Code Variables")
		FLinearColor ColorRGBA;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Code Variables")
		UMaterial *FoundMat;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Code Variables")
	//FDetachmentTransformRules detach;

	FRotator TeleportRotator;
	bool LastIsValidTeleportDestination;
	bool HasValidTeleportLocation;
	uint32 lastPoint;

	virtual void OnConstruction(const FTransform & Transform) override;//done

	UFUNCTION()//done
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void RumbleController(float intensity);//done

	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void DrawTheSpline(bool openGate, int &save, std::vector<SplinePointRegister> &splinePar);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void GrabActor();//done

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void ReleaseActor();//done

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
		void AirDrawing(FVector &WorldLocation, bool isClicked);


	//UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Default")
	AActor* GetActorNearHand();//done

	void ActivateTeleporter();//done
	void DisableTeleporter();//done

	void GetTeleporterDestination(FVector &OutLocation, FRotator &OutRotation);//done
	bool TraceTeleportDestination(TArray<FVector> &TracePoints, FVector &NavMeshLocation, FVector &TraceLocation);//done

	FRotator GetControllerRelativeRotation();
};
