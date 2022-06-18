#pragma once
#include "Polgyom.h"
#include <boost/geometry/geometry.hpp>

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;

using namespace std;

void getPointData(BoostPoint pt, double &x, double &y);

void makeThePolygonShape(Polygom &polyShape, BoostPolygon &bgPlogom);
void makeThePolygonShape(string &shape, BoostPolygon &bgPlogom);

void makeLine(Polygom &polyShape, BoostLineString &bgLineStr);


