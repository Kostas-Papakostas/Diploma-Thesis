// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickupable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupable : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class GBRUSH_CPP_API IPickupable
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickupable Interface")
		void Pickup(class USceneComponent *AttachTo);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickupable Interface")
		void Drop();

};
