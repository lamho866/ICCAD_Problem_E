#pragma once
#include "Polgyom.h"
#include "Common.h"
#include "SilkscreenOutput.h"

#include <fstream>
#include <string>
#include <boost/geometry/geometry.hpp>

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;


void checkoutPutResult(string fName, Polygom &assembly, BoostPolygon &bgAssembly, BoostMultipolygon &multBGCropper, BoostMultipolygon &cropperMulLsBuffer, vector<BoostPolygon> &cycleList);

void resultSample(string fName, Polygom assembly, BoostPolygon &bgAssembly, BoostMultipolygon &multBGCropper, BoostMultipolygon &cropperMulLsBuffer, vector<BoostPolygon> &cycleList, vector<BoostLineString> &bgDiff, double silkscreenlen);
