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
#include <boost/geometry.hpp>
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

	//printf("\n\n+------------------------------------------------------------------+\n\n");
	assembly.cyclePtCombe();
	SilkScreenOutput silkScreenOutput(silkscreenlen, assemblygap, croppergap, bgAssembly, assembly.cyclePt, assemblyLs, cropperMulLsBuffer, multBGCropper);
	
	silkScreenOutput.ResultOutput(rFile, assembly, multBGCropper, bgDiff);
	//printf("SilkScreen CanWrite Size: %d\n", silkScreenOutput.canWrite.size());
	for (double addSafety = 0.00005; addSafety <= 0.0100; addSafety += 0.00005) {
		if (!silkScreenOutput.isNeedModifty()) break;
		//printf("new-------------------------- %lf\n", addSafety);

		buildAssemblyLine(assembly, assemblygap + addSafety, multiCropperLs, croppergap + addSafety, cropperMulLsBuffer, bgDiff);
		silkScreenOutput.modiftyTheIllegalSk(assembly, addSafety, bgDiff);
	}

	multiCropperBuffer(multiCropperLs, croppergap + 0.00005, cropperMulLsBuffer);
	/*
	if (silkScreenOutput.legalSk.size() == 0) {
		printf("legalSilkscreen is 0!!!!!!\n");
		printf("=====> use illegalSk <=======\n");
		silkScreenOutput.legalSk = silkScreenOutput.illegalSk;
	}*/
	/*for (int i = 0; i < silkScreenOutput.illegalSk.size(); ++i)
		silkScreenOutput.legalSk.push_back(silkScreenOutput.illegalSk[i]);
		*/
	silkScreenOutput.skStCoordSafety();
	silkScreenOutput.write(rFile);

	//Final process way, if legalSk == 0
	if (!silkScreenOutput.isLegalSkValue()) {
		printf("\n+===============------------------------===============+\n");
		printf("+=====----------Using Final Process!!!!!----------=====+\n");
		printf("+===============------------------------===============+\n\n");
		BoostLineString outerLs;
		buildTheCombineLine(assemblygap, croppergap, bgAssembly, cropperMulLsBuffer, outerLs);
		if (outerLs.size() != 0) {
			silkScreenOutput.finalLegalWay(outerLs);
			silkScreenOutput.write(rFile);
		}
	}
	
	//GraphDraw
	checkoutPutResult(rFile, assembly, bgAssembly, multBGCropper, cropperMulLsBuffer, silkScreenOutput.cycleList);
	resultSample(rFile, assembly, bgAssembly, multBGCropper, cropperMulLsBuffer, silkScreenOutput.cycleList, bgDiff, silkscreenlen);

	//ScoreCheck
	ScoreCheck scoreCheck(pFile, rFile);
	scoreCheck.showScoreResult();
	/*
	printf("\n\n");
	for (int i = 0; i < assembly.cyclePt.size(); ++i) {
		Cycle &tempC = assembly.cyclePt[i];
		printf("c(%.4lf, %.4lf), stDeg: %.4lf, edDeg: %.4lf\n", tempC.rx, tempC.ry, tempC.stDeg, tempC.edDeg);
	}
	*/


	/*
	ofstream resultFile(rFile + ".txt");
	vector<SilkSet> &illegalSk = silkScreenOutput.illegalSk;
	printf("\n\n");
	for (int i = 0; i < illegalSk.size(); ++i) {
		SilkSet skSt = illegalSk[i];
		double assDist, cropDist;
		printf("illegal[%d]:\n", i);
		printf("line: %d, arc:%d\n", skSt.line, skSt.arc);
		BoostLineString lineStr = skSt.bgLineStr();
		skCropIsUnValue(lineStr, multBGCropper, croppergap, cropDist);
		skAssIsUnValue(lineStr, bgAssembly, assemblygap, assDist);
		printf("assDist: %lf, cropDist: %lf\n\n", assDist, cropDist);

		resultFile << "silkscreen\n";
		skSt.write(resultFile);
	}
	resultFile.close();
	*/
	system("pause");
}
