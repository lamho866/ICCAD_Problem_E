#pragma once
#include "Cycle.h"
#include "Common.h"
#include "SilkSet.h"
#include "Polygom.h"
#include "SilkscreenOutput/DropLs.h"
#include "SilkscreenOutput/OutputSilkscreen.h"
#include "SilkscreenOutput/SafetyWithCrop.h"
#include "ScoreCheck/SkValueCheck.h"

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
	void skStCoordSetUp();
	bool isIlegealAddLine(double x1, double y1, double x2, double y2);

public:
	double silkscreenlen, assemblygap, cropGap, addSafety;
	vector<BoostPolygon> cycleList;
	vector<SilkSet> skSt , legalSk, illegalSk;
	vector<Cycle> &cyclePt;
	vector<bool> canWrite;
	double as_max_x, as_max_y, as_min_x, as_min_y;
	double skSt_max_x, skSt_max_y, skSt_min_x, skSt_min_y;
	BoostMultipolygon &cropperMulLsBufferRef, &multBGCropperRef;
	BoostPolygon &bgAssemblyRef;

	SilkScreenOutput(double _silkscreenlen, double _assemblygap, double _cropGap, double _addSafety, BoostPolygon &bgAssembly, vector<Cycle> &cyclePoint, BoostLineString assemblyLs, BoostMultipolygon &cropperMulLsBuffer, BoostMultipolygon &multBGCropper);

	void makeCycleEachPoint(vector<Cycle> &cyclePt, const double assemblyGap, vector<BoostPolygon> &cycleList);

	void ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff);

	void skStCoordSafety();

	void addCoordSafety_Y(double addedY, bool isLower);

	void addCoordSafety_X(double addedX, bool isLower);

	void addCoordSafetyLine(BoostLineString &addLs, vector<BoostLineString> &cropDiff);

	void classifyLegal();
};
