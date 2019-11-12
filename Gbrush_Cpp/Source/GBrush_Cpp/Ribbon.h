// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ribbon.generated.h"

UCLASS()
class GBRUSH_CPP_API ARibbon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARibbon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class USceneComponent *Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USphereComponent *spherePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystemComponent *ribbon;
	
};
