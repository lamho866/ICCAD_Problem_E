#pragma once
#include "../Cycle.h"
#include "../Common.h"
#include "../SilkSet.h"
#include "cmath"
#include <vector>
#include <fstream>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/geometries.hpp>

using namespace std;

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;

void cropOuterLs(BoostPolygon crop, BoostLineString &outerLs);
void specialCutWithCrop(BoostPolygon crop, double tagA, double tagB, bool isX, vector<BoostLineString> &cropDiff);
void makeSafetyWithCrop(BoostPolygon crop, double tagX, double tagY, vector<BoostLineString> &cropDiff);

bool cutState(double x, double y, double tag, double isX);

void cutLineByAxis(BoostLineString ls, double tag, bool isX , bool isSecond, vector<BoostLineString> &temp);

void cutCrop(double tag, bool isX, bool isSecond, vector<BoostLineString> &cropDiff);

double linearSolOf_Y(double x1, double y1, double x2, double y2, double x);

double linearSolOf_X(double x1, double y1, double x2, double y2, double y);
