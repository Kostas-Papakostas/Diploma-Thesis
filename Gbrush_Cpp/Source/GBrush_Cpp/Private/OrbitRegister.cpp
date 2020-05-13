// Fill out your copyright notice in the Description page of Project Settings.

#include "OrbitRegister.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

OrbitRegister::OrbitRegister()
{
	OrbitsPointsSet.empty();
}

void OrbitRegister::addOrbitToVector(std::vector<SplinePointRegister> spline) {
	OrbitsPointsSet.push_back(spline);
}

std::vector<std::vector<SplinePointRegister>> OrbitRegister::getOrbitsPointsSet() {
	return OrbitsPointsSet;
}

int OrbitRegister::printToFile() {
	ofstream output;
	ifstream checkExistance;
	string path = "D:/Unreal Projects/GBrush_Cpp/output";
	path.append(".scr");
	
	checkExistance.open(path.c_str());
	int fileIndex=0;

	while (checkExistance.good()) {
		checkExistance.close();
		fileIndex++;
		path.clear();
		path = "D:/Unreal Projects/GBrush_Cpp/output_"+to_string(fileIndex);
		path.append(".scr");
		checkExistance.open(path.c_str());
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), UTF8_TO_TCHAR(path.c_str()));
	checkExistance.close();
	output.open(path.c_str(), std::ios_base::out);

	if (output.fail()) {
		std::cout << "An error occured while opening the file";
		return -1;
	}
	//for (int i = 0; i < OrbitsPointsSet.size();i++){
	output << "spline\n";
	for (std::vector<SplinePointRegister> orbit : OrbitsPointsSet) {
		for (SplinePointRegister point : orbit) {
			//print the point's location and tangent
			//on each line
			FString splineP = point.getSplinePoints().ToString(); //spline point
			FString splineT = point.getPointsTangent().ToString(); //spline point tangent

			//PRINT SPLINE POINT COORD
			splineP=splineP.Replace(TEXT(" "), TEXT(","), ESearchCase::IgnoreCase);
			splineP = splineP.Replace(TEXT("X="), TEXT(""), ESearchCase::CaseSensitive);
			splineP = splineP.Replace(TEXT("Y="), TEXT(""), ESearchCase::CaseSensitive);
			splineP = splineP.Replace(TEXT("Z="), TEXT(""), ESearchCase::CaseSensitive);

			//PRINT SPLINE POINT TANGENT 
			splineT=splineT.Replace(TEXT(" "), TEXT(","), ESearchCase::IgnoreCase);
			splineT = splineT.Replace(TEXT("X="), TEXT(""), ESearchCase::CaseSensitive);
			splineT = splineT.Replace(TEXT("Y="), TEXT(""), ESearchCase::CaseSensitive);
			splineT = splineT.Replace(TEXT("Z="), TEXT(""), ESearchCase::CaseSensitive);

			if (output.is_open()) {
				output << TCHAR_TO_ANSI(*splineP) << endl;// << TCHAR_TO_ANSI(*splineT) << endl;
			}
		}
		//newline for the next orbit
		output << endl;
	}

	output.close();
	return 0;
}
//OrbitRegister::~OrbitRegister()
//{
//}
