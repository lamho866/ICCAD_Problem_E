#pragma once
#include "Common.h"
#include "Polgyom.h"
#include "SilkscreenMaker.h"

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

class LineArc
{
public:
	int line, arc;
	LineArc() : line(0), arc(0) {}
	void readLine(string s);
};

class ScoreCheck {
private:
	string pFile, rFile;
	Polygom assembly;
	BoostPolygon bgAssembly;
	vector<Polygom> cropperList;
	BoostMultipolygon multBGCropper;
	LineArc problemLA, resultLA;
	BoostMultiLineString assemblyMultLine, resultLs;

	double assemblygap, croppergap, silkscreenlen;

	double scoreCal(double score, double percent);
	double avgCropperDistance(vector<BoostLineString> &resultLs, BoostMultipolygon &multBGCropper);
	double score_assemblyAreaBuffer();
	void readLineData(string s, double &x1, double &y1, double &x2, double &y2);

	void scoreCase1();
	void scoreCase2(BoostLineString &assemblyLs);
	void scoreCase3();
	void scoreCase4();
	void lineConnectTest();
	void silkScreenLenTest();
public:
	ScoreCheck(string &_pFile, string &_rFile);
	void readContestFile();
	void readResultFile();
	void showScoreResult();
};