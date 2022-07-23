#pragma once
#include "Cycle.h"
#include "Common.h"
#include "SilkSet.h"
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
	void isCoordEquals(double max_cr, double cur_max, int &cnt);
	void cntMaxMin(double max_x, double max_y, double min_x, double min_y, int &max_x_cnt, int &max_y_cnt, int &min_x_cnt, int &min_y_cnt, SilkSet &sk);
	void skStCoordSetUp();
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

	SilkScreenOutput(double _silkscreenlen, double _assemblygap, vector<Cycle> &cyclePoint, BoostLineString assemblyLs);

	void makeCycleEachPoint(vector<Cycle> &cyclePt, const double assemblyGap, vector<BoostPolygon> &cycleList);

	void intputCycle(BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls, int cIdx, int &i);

	bool collinear(BoostPoint pt1, BoostPoint pt2, BoostPoint pt3);

	void drawLine(BoostLineString &ls, int &i);

	void outputSilkscreen(BoostLineString &ls, vector<Cycle> assemblyCycleList);

	void ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff);

	void dropLs();

	void silkCoordMaxXSafety();

	void silkCoordMaxYSafety();

	void silkCoordMinXSafety();

	void silkCoordMinYSafety();
};
