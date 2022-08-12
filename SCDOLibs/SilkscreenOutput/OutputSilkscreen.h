#pragma once
#include "../Cycle.h"
#include "../Common.h"
#include "../SilkSet.h"
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

void outputSilkscreen(vector<SilkSet> &skSt, BoostLineString &ls, vector<Cycle> assemblyCycleList, vector<Cycle> &cyclePt, vector<BoostPolygon> &cycleList);

void drawLine(BoostLineString &ls, int &i, SilkSet &sk);

void intputCycle(BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls, int cIdx, int &i, SilkSet &sk);

bool collinear(BoostPoint pt1, BoostPoint pt2, BoostPoint pt3);

void arcCombine(SilkSet &skLs);

