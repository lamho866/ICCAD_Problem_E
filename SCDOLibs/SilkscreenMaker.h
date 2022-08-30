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
typedef bg::model::ring<BoostPoint> BoostRing;

using namespace std;

template <typename T>
int countStartPoint(T &ls, const double tagX, const double tagY);

template <typename T>
void removeConnectPoint(T &ls, const double tagX, const double tagY);

template <typename T>
void modifyStartPoint(T &ls, const double tagX, const double tagY);

template <typename T>
void startPointRestruct(BoostLineString &originLs, T &ls);

struct SegmentLine;

void connectAB(SegmentLine &a, SegmentLine &b, double &res_x, double &res_y);

void assemblyBuffer(Polygom &assembly, const double assemblygap, BoostMultiLineString &assemblyMultLine);

void multiCropperBuffer(BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer);

void connectLine(vector<BoostLineString> &bgDiff);

void buildAssemblyLine(Polygom &assembly, const double assemblygap, BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer, vector<BoostLineString> &bgDiff);

void buildTheCombineLine(double assGap, double cropGap, BoostPolygon bgAssembly, BoostMultipolygon multBGCropper, BoostLineString &coverLs);
