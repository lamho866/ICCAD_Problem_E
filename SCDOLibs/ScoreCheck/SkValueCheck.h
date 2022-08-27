#pragma once
#include <boost/geometry/geometry.hpp>

using namespace std;

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;

bool skCropIsValue(BoostLineString &sk, BoostMultipolygon &multBGCropper,double cropGap,  double &minDist);

bool skAssIsValue(BoostLineString &sk, BoostPolygon &bgAssembly, double assGap, double &minDist);