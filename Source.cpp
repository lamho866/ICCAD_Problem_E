#define _CRT_SECURE_NO_WARNINGS
#include "SCDOLibs/Common.h"
#include "SCDOLibs/Cycle.h"
#include "SCDOLibs/Polygom.h"
#include "SCDOLibs/SilkscreenMaker.h"
#include "SCDOLibs/SilkscreenOutput.h"
#include "SCDOLibs/GraphDraw.h"
#include "SCDOLibs/ScoreCheck.h"

#include <iostream>
#include <string>
#include <fstream>
#include <boost/geometry/geometry.hpp>

using namespace std;

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;


int main()
{
	//string pFile = "PublicCase/PublicCase_A";
	//string rFile = "ResultPublicCase/Result_A";
	

	string pFile = "Problem";
	string rFile = "Result";

	Polygom assembly;
	BoostPolygon bgAssembly;
	BoostLineString assemblyLs;

	vector<Polygom> cropperList;
	BoostMultipolygon multBGCropper;
	BoostMultiLineString multiCropperLs;
	BoostPolygon cropper;
	BoostLineString cropperLs;

	string str;
	double assemblygap, croppergap, silkscreenlen;
	int cropSize = 0;
	std::ifstream input(pFile + ".txt");
	input >> str;
	assemblygap = atof(str.substr(12).c_str());
	input >> str;
	croppergap = atof(str.substr(10).c_str());
	input >> str;
	silkscreenlen = atof(str.substr(14).c_str());
	input >> str;

	croppergap += 0.0001;
	assemblygap += 0.0001;

	//assembly
	while (1) {
		input >> str;
		if (str[0] == 'c') break;
		assembly.addLine(str);
	}
	makeThePolygonShape(assembly, bgAssembly);
	makeLine(assembly, assemblyLs);

	//cropper
	cropperList.push_back(Polygom());
	while (input >> str) {
		if (str[0] == 'c') {
			makeThePolygonShape(cropperList[cropSize], cropper);
			makeLine(cropperList[cropSize], cropperLs);
			multBGCropper.push_back(cropper);
			multiCropperLs.push_back(cropperLs);
			cropper.clear();
			cropperList.push_back(Polygom());
			cropSize++;
			continue;
		}
		cropperList[cropSize].addLine(str);
	}
	makeThePolygonShape(cropperList[cropSize], cropper);
	makeLine(cropperList[cropSize], cropperLs);
	multBGCropper.push_back(cropper);
	multiCropperLs.push_back(cropperLs);
	cropSize++;
	input.close();

	vector<BoostLineString> bgDiff;
	BoostMultipolygon cropperMulLsBuffer;
	buildAssemblyLine(assembly, assemblygap, multiCropperLs, croppergap, cropperMulLsBuffer, bgDiff);

	assembly.cyclePtCombe();
	SilkScreenOutput silkScreenOutput(silkscreenlen, assemblygap, bgAssembly, assembly.cyclePt, assemblyLs, cropperMulLsBuffer, multBGCropper);
	silkScreenOutput.ResultOutput(rFile, assembly, multBGCropper, bgDiff);


	//GraphDraw
	checkoutPutResult(rFile, assembly, bgAssembly, multBGCropper, cropperMulLsBuffer, silkScreenOutput.cycleList);
	resultSample(rFile, assembly, bgAssembly, multBGCropper, cropperMulLsBuffer, silkScreenOutput.cycleList, bgDiff, silkscreenlen);

	//ScoreCheck
	ScoreCheck scoreCheck(pFile, rFile);
	scoreCheck.showScoreResult();

	printf("AssemblySize: %d\n", assembly.cyclePt.size());

	system("pause");
}

