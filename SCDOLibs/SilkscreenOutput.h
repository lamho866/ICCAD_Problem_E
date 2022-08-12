#pragma once
#include "Cycle.h"
#include "Common.h"
#include "SilkSet.h"
#include "SilkscreenOutput/DropLs.h"
#include "SilkscreenOutput/OutputSilkscreen.h"

#include "cmath"
#include <vector>
#include <fstream>
#include <boost/geometry/geometry.hpp>

using namespace std;

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;

class SilkScreenOutput {
private:
	void write(string &fileName);
	void silkScreenModify();
	void arcLineCheck(SilkSet &sk);
	void addTurningPoint(int &i, SilkSet &skSet, bool isHeaed);
	void addArcSafetyLine(int &i, SilkSet &skSet);
	void skStCoordSetUp();
	bool isIlegealAddLine(double x1, double y1, double x2, double y2);
	//int inCycleIdx(int i, BoostLineString &ls, double x, double y);
	//bool isInMoreCycle(int i, BoostLineString &ls, int cIdx, double x, double y);
public:
	double silkscreenlen, assemblygap;
	vector<BoostPolygon> cycleList;
	vector<SilkSet> skSt;
	vector<Cycle> &cyclePt;
	vector<bool> canWrite;
	double as_max_x, as_max_y, as_min_x, as_min_y;
	double skSt_max_x, skSt_max_y, skSt_min_x, skSt_min_y;
	BoostMultipolygon &cropperMulLsBufferRef, &multBGCropperRef;
	BoostPolygon &bgAssemblyRef;

	SilkScreenOutput(double _silkscreenlen, double _assemblygap, BoostPolygon &bgAssembly, vector<Cycle> &cyclePoint, BoostLineString assemblyLs, BoostMultipolygon &cropperMulLsBuffer, BoostMultipolygon &multBGCropper);

	void makeCycleEachPoint(vector<Cycle> &cyclePt, const double assemblyGap, vector<BoostPolygon> &cycleList);

	void ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff);

	void skStCoordSafety();

	void addCoordSafety_Y(double addedY);

	void addCoordSafety_X(double addedX);
};
