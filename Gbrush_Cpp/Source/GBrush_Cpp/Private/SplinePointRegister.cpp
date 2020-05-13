// Fill out your copyright notice in the Description page of Project Settings.

#include "SplinePointRegister.h"
#include "GBrush_Cpp.h"

SplinePointRegister::SplinePointRegister()
{
	splinePoints = FVector(0.f, 0.f, 0.f);
	pointsTangent = FVector(0.f, 0.f, 0.f);
	pointObject objPoint = {};
}

/*SplinePointRegister::~SplinePointRegister()
{
}*/


void SplinePointRegister::assignSplinePoint(FVector point, FVector tangent) {
	pointObject objPoint = { point[0], point[1], point[2], tangent };
	splinePoints = point;
	pointsTangent = tangent;
}

FVector SplinePointRegister::getPointsTangent() {
	return pointsTangent;
}

FVector SplinePointRegister::getSplinePoints() {
	return splinePoints;
}

