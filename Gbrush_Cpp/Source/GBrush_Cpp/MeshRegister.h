#pragma once

#include "CoreMinimal.h"
#include "SculptingMeshes/SphereSculpting.h"
#include "SculptingMeshes/CylinderSculpting.h"
#include "SculptingMeshes/ConeSculpting.h"
#include "SculptingMeshes/CubeSculpting.h"
#include "GBrush_Cpp.h"
#include <vector>

class GBRUSH_CPP_API MeshRegister
{
private:
	std::vector<ASphereSculpting*> SphereArray;
	std::vector<ACubeSculpting*> CubeArray;
	std::vector<AConeSculpting*> ConeArray;
	std::vector<ACylinderSculpting*> CylinderArray;
public:
	MeshRegister();
	//~MeshRegister();

	void addCube(ACubeSculpting* aCube);
	void addCone(AConeSculpting* aCone);
	void addCylinder(ACylinderSculpting* aCylinder);
	void addSphere(ASphereSculpting* aSphere);

	int printEverything();
};

