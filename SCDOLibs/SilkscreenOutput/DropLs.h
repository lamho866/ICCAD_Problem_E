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

void dropLs(vector<bool> &canWrite, vector<SilkSet> &skSt);

void cntMaxMin(double max_x, double max_y, double min_x, double min_y, int &max_x_cnt, int &max_y_cnt, int &min_x_cnt, int &min_y_cnt, SilkSet &sk);

void isCoordEquals(double max_cr, double cur_max, int &cnt);
