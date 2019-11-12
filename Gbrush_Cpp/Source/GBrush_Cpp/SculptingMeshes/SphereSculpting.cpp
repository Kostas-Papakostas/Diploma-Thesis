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
		//UE_LOG(LogTemp, Error, TEXT("Index is %s"), *FString(FString::FromInt(i)));
		//UE_LOG(LogTemp, Warning, TEXT("Result is %s"), *VerticesArray.operator[](i).ToString());
		FVector temp = VerticesArray.operator[](i);
		
		//UE_LOG(LogTemp, Warning, TEXT("Temp is %s"), *temp.ToString());
		
		//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="temp",meta=(AllowPrivateAccess="true"))
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
	
	//ProceduralMeshPoints.Empty();
	//VerticesLocationDivider = 0.2;//ProceduralSphere->GetComponentScale().GetAbsMax();


	//for (int32 i = 0; i < VerticesArray.Num(); i++) {
	//	//UE_LOG(LogTemp, Error, TEXT("Index is %s"), *FString(FString::FromInt(i)));
	//	//UE_LOG(LogTemp, Warning, TEXT("Result is %s"), *VerticesArray.operator[](i).ToString());
	//	FVector temp = VerticesArray.operator[](i);

	//	//UE_LOG(LogTemp, Warning, TEXT("Temp is %s"), *temp.ToString());

	//	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="temp",meta=(AllowPrivateAccess="true"))
	//	UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*FString("Node" + FString::FromInt(i)));

	//	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));

	//	if (CubeAsset.Succeeded()) {

	//		MeshComponent->SetStaticMesh(CubeAsset.Object);
	//		MeshComponent->SetRelativeScale3D(FVector(0.05f));
	//		MeshComponent->SetRelativeLocation(temp);
	//		MeshComponent->SetVisibility(CanBeSeen, false);
	//		MeshComponent->SetupAttachment(MeshOrigin);


	//		ProceduralMeshPoints.Insert(MeshComponent, i);
	//		//UE_LOG(LogTemp, Warning, TEXT("Point Location is %s"), *ProceduralMeshPoints.operator[](i)->K2_GetComponentLocation().ToString());
	//	}
	//}
}

// Called every frame
void ASphereSculpting::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*for (int32 i = 0; i < ProceduralMeshPoints.Num(); i++) {
	UKismetSystemLibrary::PrintString(this, *FString(ProceduralMeshPoints.operator[](i)->GetName()), true, false);
	}*/
	//VerticesArray.Empty();
	for (int32 i = 0; i < ProceduralMeshPoints.Num(); i++) {
		//	if (GEngine) {
		//		FString tt = FString::SanitizeFloat(VerticesLocationDivider);
		//		UE_LOG(LogTemp, Warning, TEXT("Name is %s"), *FString(ProceduralMeshPoints.operator[](i)->GetName()));
		//		GEngine->AddOnScreenDebugMessage(12, 1.f, FColor::Red, *FString(ProceduralMeshPoints.operator[](i)->GetName()), false);
		//	}
		//UE_LOG(LogTemp, Error, TEXT("Index is %s"), *FString(FString::FromInt(i)));
		//UE_LOG(LogTemp, Warning, TEXT("Vertex point is %s"), *VerticesArray.operator[](i).ToString());
		FVector resultLocation = (ProceduralMeshPoints.operator[](i))->GetRelativeTransform().GetLocation();
		//UE_LOG(LogTemp, Warning, TEXT("Result Point is %s"), *resultLocation.ToString());
		//UKismetSystemLibrary::PrintString(this, *FString(resultLocation.ToString()), true, false);
		VerticesArray.RemoveAt(i, 1, false);
		VerticesArray.Insert(resultLocation, i);

		//UKismetSystemLibrary::PrintString(this, *FString(VerticesArray.operator[](i).ToString()), true, false);
		ProceduralSphere->UpdateMeshSection(0, VerticesArray, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>());

	}
	ProceduralSphere->UpdateMeshSection(0, VerticesArray, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>());
	UKismetSystemLibrary::PrintString(this, *FString(FString::FromInt(VerticesArray.Num())), true, false);
	//VerticesLocationDivider = ProceduralSphere->GetComponentScale().GetAbsMax();

}

void ASphereSculpting::setCanBeSeen(bool preferedValue) {
	CanBeSeen = preferedValue;
}

bool ASphereSculpting::getCanBeSeen() {
	return CanBeSeen;
}
