#include "MeshRegister.h"
#include <iostream>
#include <string>
#include <fstream>

//#include "SculptingMeshes/SphereSculpting.h"
//#include "SculptingMeshes/CylinderSculpting.h"
//#include "SculptingMeshes/ConeSculpting.h"
//#include "SculptingMeshes/CubeSculpting.h"



using namespace std;

MeshRegister::MeshRegister()
{
	SphereArray.empty();
	CubeArray.empty();
	ConeArray.empty();
	CylinderArray.empty();
}


void MeshRegister::addCube(ACubeSculpting* aCube) {
	CubeArray.push_back(aCube);
}

void MeshRegister::addCone(AConeSculpting* aCone) {
	ConeArray.push_back(aCone);
}

void MeshRegister::addCylinder(ACylinderSculpting* aCylinder) {
	CylinderArray.push_back(aCylinder);
}

void MeshRegister::addSphere(ASphereSculpting* aSphere) {
	SphereArray.push_back(aSphere);
}

int MeshRegister::printEverything() {

	//print ALL spheres
	for (int i = 0; i < SphereArray.size(); i++) {
		ofstream outputSphere;
		ifstream checkExistanceSphere;
		string path = "D:/Unreal Projects/GBrush_Cpp/output_Sphere_"+to_string(i);
		path.append(".obj");

		checkExistanceSphere.open(path.c_str());
		int existanceCase = 0;
		
		while (checkExistanceSphere.good()) {
			checkExistanceSphere.close();
			existanceCase += 1;
			path.clear();
			path = "D:/Unreal Projects/GBrush_Cpp/output_Sphere_" + to_string(i) + "(" + to_string(existanceCase) + ")";
			path.append(".obj");
			checkExistanceSphere.open(path.c_str());
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(path.c_str()));
		checkExistanceSphere.close();
		outputSphere.open(path.c_str(), std::ios_base::out);

		if (outputSphere.fail()) {
			std::cout << "An error occured while opening the file";
			return -1;
		}

		outputSphere << "# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware\n# File Created : 28.02.2019 20 : 40 : 23\n\n#\n# object Sphere001\n#\n\n";/*+ "mtllib sph.mtl\n\n"*/

		for (int v = 0; v < SphereArray.operator[](i)->getVerticesArray().Num(); v++) {
			double x = SphereArray.operator[](i)->getVerticesArray().operator[](v).X;
			double y = SphereArray.operator[](i)->getVerticesArray().operator[](v).Y;
			double z = SphereArray.operator[](i)->getVerticesArray().operator[](v).Z;
			outputSphere << "v " << x << " " << y << " " << z << "\n";
		}
		outputSphere << "# " << SphereArray.operator[](i)->getVerticesArray().Num() << " Vertices\n\n";
		
		outputSphere << "g Sphere001\n" /*+ "usemtl wire_088144225\n"*/;
		for (int f = 0; f < SphereArray.operator[](i)->getTrianglesArray().Num(); f++) {
			int32 first = SphereArray.operator[](i)->getTrianglesArray().operator[](f) + 1;
			int32 second = SphereArray.operator[](i)->getTrianglesArray().operator[](f + 2) + 1;
			int32 third = SphereArray.operator[](i)->getTrianglesArray().operator[](f + 1) + 1;
			outputSphere << "f " << first << " " << second << " " << third << "\n";
		}
		outputSphere << "# 0 polygons - " << SphereArray.operator[](i)->getTrianglesArray().Num() << " triangles\n\n";

	}

	//for (int i = 0; i < CubeArray.size(); i++) {
	//	ofstream outputCube;
	//	ifstream checkExistanceCube;
	//	string path = "D:/Unreal Projects/GBrush_Cpp/output_Cube_" + to_string(i);
	//	path.append(".obj");

	//	checkExistanceCube.open(path.c_str());
	//	int existanceCase = 0;

	//	while (checkExistanceCube.good()) {
	//		checkExistanceCube.close();
	//		existanceCase += 1;
	//		path.clear();
	//		path = "D:/Unreal Projects/GBrush_Cpp/output_Cube_" + to_string(i) + "(" + to_string(existanceCase) + ")";
	//		path.append(".obj");
	//		checkExistanceCube.open(path.c_str());
	//	}
	//	UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(path.c_str()));
	//	checkExistanceCube.close();
	//	outputCube.open(path.c_str(), std::ios_base::out);

	//	if (outputCube.fail()) {
	//		std::cout << "An error occured while opening the file";
	//		return -1;
	//	}

	//	outputCube << "# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware\n# File Created : 28.02.2019 20 : 40 : 23\n\n#\n# object Cube001\n#\n\n";/*+ "mtllib sph.mtl\n\n"*/

	//	for (int v = 0; v < CubeArray.operator[](i)->getVerticesArray().Num(); v++) {
	//		double x = CubeArray.operator[](i)->getVerticesArray().operator[](v).X;
	//		double y = CubeArray.operator[](i)->getVerticesArray().operator[](v).Y;
	//		double z = CubeArray.operator[](i)->getVerticesArray().operator[](v).Z;
	//		outputCube << "v " << x << " " << y << " " << z << "\n";
	//	}
	//	outputCube << "# " << CubeArray.operator[](i)->getVerticesArray().Num() << " Vertices\n\n";

	//	outputCube << "g Cube001\n" /*+ "usemtl wire_088144225\n"*/;
	//	for (int f = 0; f < CubeArray.operator[](i)->getTrianglesArray().Num(); f++) {
	//		int32 first = CubeArray.operator[](i)->getTrianglesArray().operator[](f) + 1;
	//		int32 second = CubeArray.operator[](i)->getTrianglesArray().operator[](f + 2) + 1;
	//		int32 third = CubeArray.operator[](i)->getTrianglesArray().operator[](f + 1) + 1;
	//		outputCube << "f " << first << " " << second << " " << third << "\n";
	//	}
	//	outputCube << "# 0 polygons - " << CubeArray.operator[](i)->getTrianglesArray().Num() << " triangles\n\n";

	//}

	//for (int i = 0; i < ConeArray.size(); i++) {
	//	ofstream outputCone;
	//	ifstream checkExistanceCone;
	//	string path = "D:/Unreal Projects/GBrush_Cpp/output_Cone_" + to_string(i);
	//	path.append(".obj");

	//	checkExistanceCone.open(path.c_str());
	//	int existanceCase = 0;

	//	while (checkExistanceCone.good()) {
	//		checkExistanceCone.close();
	//		existanceCase += 1;
	//		path.clear();
	//		path = "D:/Unreal Projects/GBrush_Cpp/output_Cone_" + to_string(i) + "(" + to_string(existanceCase) + ")";
	//		path.append(".obj");
	//		checkExistanceCone.open(path.c_str());
	//	}
	//	UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(path.c_str()));
	//	checkExistanceCone.close();
	//	outputCone.open(path.c_str(), std::ios_base::out);

	//	if (outputCone.fail()) {
	//		std::cout << "An error occured while opening the file";
	//		return -1;
	//	}

	//	outputCone << "# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware\n# File Created : 28.02.2019 20 : 40 : 23\n\n#\n# object Cone001\n#\n\n";/*+ "mtllib sph.mtl\n\n"*/

	//	for (int v = 0; v < ConeArray.operator[](i)->getVerticesArray().Num(); v++) {
	//		double x = ConeArray.operator[](i)->getVerticesArray().operator[](v).X;
	//		double y = ConeArray.operator[](i)->getVerticesArray().operator[](v).Y;
	//		double z = ConeArray.operator[](i)->getVerticesArray().operator[](v).Z;
	//		outputCone << "v " << x << " " << y << " " << z << "\n";
	//	}
	//	outputCone << "# " << ConeArray.operator[](i)->getVerticesArray().Num() << " Vertices\n\n";

	//	outputCone << "g Cone001\n" /*+ "usemtl wire_088144225\n"*/;
	//	for (int f = 0; f < ConeArray.operator[](i)->getTrianglesArray().Num(); f++) {
	//		int32 first = ConeArray.operator[](i)->getTrianglesArray().operator[](f) + 1;
	//		int32 second = ConeArray.operator[](i)->getTrianglesArray().operator[](f + 2) + 1;
	//		int32 third = ConeArray.operator[](i)->getTrianglesArray().operator[](f + 1) + 1;
	//		outputCone << "f " << first << " " << second << " " << third << "\n";
	//	}
	//	outputCone << "# 0 polygons - " << ConeArray.operator[](i)->getTrianglesArray().Num() << " triangles\n\n";

	//}

	//for (int i = 0; i < CylinderArray.size(); i++) {
	//	ofstream outputCylinder;
	//	ifstream checkExistanceCylinder;
	//	string path = "D:/Unreal Projects/GBrush_Cpp/output_Cylinder_" + to_string(i);
	//	path.append(".obj");

	//	checkExistanceCylinder.open(path.c_str());
	//	int existanceCase = 0;

	//	while (checkExistanceCylinder.good()) {
	//		checkExistanceCylinder.close();
	//		existanceCase += 1;
	//		path.clear();
	//		path = "D:/Unreal Projects/GBrush_Cpp/output_Cylinder_" + to_string(i) + "(" + to_string(existanceCase) + ")";
	//		path.append(".obj");
	//		checkExistanceCylinder.open(path.c_str());
	//	}
	//	UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(path.c_str()));
	//	checkExistanceCylinder.close();
	//	outputCylinder.open(path.c_str(), std::ios_base::out);

	//	if (outputCylinder.fail()) {
	//		std::cout << "An error occured while opening the file";
	//		return -1;
	//	}

	//	outputCylinder << "# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware\n# File Created : 28.02.2019 20 : 40 : 23\n\n#\n# object Cylinder001\n#\n\n";/*+ "mtllib sph.mtl\n\n"*/

	//	for (int v = 0; v < CylinderArray.operator[](i)->getVerticesArray().Num(); v++) {
	//		double x = CylinderArray.operator[](i)->getVerticesArray().operator[](v).X;
	//		double y = CylinderArray.operator[](i)->getVerticesArray().operator[](v).Y;
	//		double z = CylinderArray.operator[](i)->getVerticesArray().operator[](v).Z;
	//		outputCylinder << "v " << x << " " << y << " " << z << "\n";
	//	}
	//	outputCylinder << "# " << CylinderArray.operator[](i)->getVerticesArray().Num() << " Vertices\n\n";

	//	outputCylinder << "g Cylinder001\n" /*+ "usemtl wire_088144225\n"*/;
	//	for (int f = 0; f < CylinderArray.operator[](i)->getTrianglesArray().Num(); f++) {
	//		int32 first = CylinderArray.operator[](i)->getTrianglesArray().operator[](f) + 1;
	//		int32 second = CylinderArray.operator[](i)->getTrianglesArray().operator[](f + 2) + 1;
	//		int32 third = CylinderArray.operator[](i)->getTrianglesArray().operator[](f + 1) + 1;
	//		outputCylinder << "f " << first << " " << second << " " << third << "\n";
	//	}
	//	outputCylinder << "# 0 polygons - " << CylinderArray.operator[](i)->getTrianglesArray().Num() << " triangles\n\n";

	//}


	return 0;
}


//MeshRegister::~MeshRegister()
//{
//}
