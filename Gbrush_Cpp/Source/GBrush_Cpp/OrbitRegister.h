// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SplinePointRegister.h"
#include <vector>

/**
 * 
 */
class GBRUSH_CPP_API OrbitRegister
{
private:
	std::vector<std::vector<SplinePointRegister>> OrbitsPointsSet;

public:
	OrbitRegister();
	//~OrbitRegister();

	std::vector<std::vector<SplinePointRegister>> getOrbitsPointsSet();
	void setOrbitsPointsSet(SplinePointRegister pointsSum);
	void addOrbitToVector(std::vector<SplinePointRegister> pointsSum);
	int printToFile(void);
};
