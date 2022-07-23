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

bool isLargerEnough(BoostLineString silkScreen, const double silkscreenlen);

void rtPt(double curX, double curY, double deg, double &nxtX, double &nxtY);

bool almost_equal(double x, double y);

void findCoordMaxMin(BoostLineString ls, double &max_x, double &max_y, double &min_x, double &min_y);
