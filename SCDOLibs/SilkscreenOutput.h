#pragma once
#include "Cycle.h"
#include "Common.h"
#include "SilkSet.h"
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
public:
	double silkscreenlen, assemblygap;
	vector<BoostPolygon> cycleList;
	vector<SilkSet> skSt;

	SilkScreenOutput(double _silkscreenlen, double _assemblygap, vector<Cycle> &cyclePt);

	void makeCycleEachPoint(vector<Cycle> &cyclePt, const double assemblyGap, vector<BoostPolygon> &cycleList);

	void intputCycle(BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls, int &i);

	bool collinear(BoostPoint pt1, BoostPoint pt2, BoostPoint pt3);

	void drawLine(BoostLineString &ls, int &i);

	void outputSilkscreen(BoostLineString &ls, vector<Cycle> assemblyCycleList);

	void ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff);
};
