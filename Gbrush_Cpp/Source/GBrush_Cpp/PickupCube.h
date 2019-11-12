// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupCube.generated.h"

UCLASS()
class GBRUSH_CPP_API APickupCube : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Code Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent *StaticMeshComponent;
public:	
	// Sets default values for this actor's properties
	APickupCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Pickupable Interface")
	void Pickup(class USceneComponent *AttachTo);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickupable Interface")
		void Drop();
};
