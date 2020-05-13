// Fill out your copyright notice in the Description page of Project Settings.

#include "SphereSculpting.h"
#include "ProceduralMeshComponent.h"
#include "ConstructorHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASphereSculpting::ASphereSculpting()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CanBeSeen = true;
	MeshOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("Mesh Origin"));
	MeshOrigin->SetupAttachment(RootComponent);

	ProceduralSphere = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh Sphere"));
	ProceduralSphere->SetupAttachment(MeshOrigin);

	ProceduralMeshPoints.Empty();
	ProceduralSphere->CreateMeshSection(0, VerticesArray, TrianglesArray, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	VerticesLocationDivider = 0.2;//ProceduralSphere->GetComponentScale().GetAbsMax();
	

	for (int32 i = 0; i<VerticesArray.Num(); i++) {

		FVector temp = VerticesArray.operator[](i);

		UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*FString("Node"+FString::FromInt(i)));
		
		static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));

		if (CubeAsset.Succeeded()) {
			
			MeshComponent->SetStaticMesh(CubeAsset.Object);
			MeshComponent->SetRelativeScale3D(FVector(0.05f));
			MeshComponent->SetRelativeLocation(temp);
			MeshComponent->SetVisibility(CanBeSeen, false);
			MeshComponent->SetupAttachment(MeshOrigin);


			ProceduralMeshPoints.Insert(MeshComponent,i);
			//UE_LOG(LogTemp, Warning, TEXT("Point Location is %s"), *ProceduralMeshPoints.operator[](i)->K2_GetComponentLocation().ToString());
		}
	}
}

TArray<UStaticMeshComponent*> ASphereSculpting::getProceduralMeshPoints() {
	return ProceduralMeshPoints;
}

TArray<int32> ASphereSculpting::getTrianglesArray() {
	return TrianglesArray;
}

TArray<FVector> ASphereSculpting::getVerticesArray() {
	return VerticesArray;
}

// Called when the game starts or when spawned
void ASphereSculpting::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ASphereSculpting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int32 i = 0; i < ProceduralMeshPoints.Num(); i++) {
	
		FVector resultLocation = (ProceduralMeshPoints.operator[](i))->GetRelativeTransform().GetLocation();
		VerticesArray.RemoveAt(i, 1, false);
		VerticesArray.Insert(resultLocation, i);

		ProceduralSphere->UpdateMeshSection(0, VerticesArray, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>());

	}
	ProceduralSphere->UpdateMeshSection(0, VerticesArray, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>());
	UKismetSystemLibrary::PrintString(this, *FString(FString::FromInt(VerticesArray.Num())), true, false);

}

void ASphereSculpting::setCanBeSeen(bool preferedValue) {
	CanBeSeen = preferedValue;
}

bool ASphereSculpting::getCanBeSeen() {
	return CanBeSeen;
}
