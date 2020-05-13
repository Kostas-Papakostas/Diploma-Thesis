// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"

/**
 * 
 */
struct pointObject
{
	float pointX;
	float pointY;
	float pointZ;
	FVector tangent;
};


class GBRUSH_CPP_API SplinePointRegister
{
private:
	FVector splinePoints;
	FVector pointsTangent;

public:
	SplinePointRegister();
	//~SplinePointRegister();

	FVector getPointsTangent();
	FVector getSplinePoints();

	void assignSplinePoint(FVector point, FVector tangent);
	
};
