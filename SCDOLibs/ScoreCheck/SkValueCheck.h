#pragma once
#include "../Common.h"
#include <boost/geometry/geometry.hpp>

using namespace std;

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;

bool skCropIsUnValue(BoostLineString &sk, BoostMultipolygon &multBGCropper,double cropGap,  double &minDist);

bool skAssIsUnValue(BoostLineString &sk, BoostPolygon &bgAssembly, double assGap, double &minDist);
