#pragma once
#include "Polygom.h"
#include "Common.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/geometries.hpp>
namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;

using namespace std;

int countStartPoint(BoostLineString &ls, const double tagX, const double tagY);

void removeConnectPoint(BoostLineString &ls, const double tagX, const double tagY);

struct SegmentLine;

void connectAB(SegmentLine &a, SegmentLine &b, double &res_x, double &res_y);

void modifyStartPoint(BoostLineString &ls, const double tagX, const double tagY);

void assemblyBuffer(Polygom &assembly, const double assemblygap, BoostMultiLineString &assemblyMultLine);

void multiCropperBuffer(BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer);

void connectLine(vector<BoostLineString> &bgDiff);

void buildAssemblyLine(Polygom &assembly, const double assemblygap, BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer, vector<BoostLineString> &bgDiff);
