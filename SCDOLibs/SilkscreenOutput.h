#pragma once
#include "Cycle.h"
#include "Common.h"
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


void makeCycleEachPoint(vector<Cycle> &cyclePt, const double assemblyGap, vector<BoostPolygon> &cycleList);

bool isLargerEnough(BoostLineString silkScreen, const double silkscreenlen);

void inputPoint(ofstream &file, BoostPoint &pt);

void inputPoint(ofstream &file, const double x, const double y);

void intputCycle(ofstream &file, BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls, int &i);

bool collinear(BoostPoint pt1, BoostPoint pt2, BoostPoint pt3);

void drawLine(ofstream &file, BoostLineString &ls, int &i);

void outputSilkscreen(ofstream &file, BoostLineString &ls, vector<BoostPolygon> &cyclePolygonList, vector<Cycle> cycleList);

void ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff, vector<BoostPolygon> &cycleList, double silkscreenlen);
