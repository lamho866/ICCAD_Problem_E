#include "Common.h"

void getPointData(BoostPoint pt, double &x, double &y) {
	x = bg::get<0>(pt);
	y = bg::get<1>(pt);
}

void makeThePolygonShape(Polygom &polyShape, BoostPolygon &bgPlogom) {
	polyShape.shape[polyShape.shape.size() - 1] = ' ';
	bg::read_wkt("POLYGON((" + polyShape.shape + "))", bgPlogom);
}

void makeThePolygonShape(string &shape, BoostPolygon &bgPlogom) {
	shape[shape.size() - 1] = ' ';
	bg::read_wkt("POLYGON((" + shape + "))", bgPlogom);
}

void makeLine(Polygom &polyShape, BoostLineString &bgLineStr) {
	polyShape.shape[polyShape.shape.size() - 1] = ' ';
	bg::read_wkt("LINESTRING(" + polyShape.shape + ")", bgLineStr);
}

bool isLargerEnough(BoostLineString silkScreen, const double silkscreenlen) {
	//cout << "Len : " << bg::length(silkScreen) << endl;
	return (bg::length(silkScreen)) > silkscreenlen;
}

void rtPt(double curX, double curY, double deg, double &nxtX, double &nxtY) {
	nxtX = curX * cos(deg*PI / 180.0) - curY * sin(deg*PI / 180.0);
	nxtY = curX * sin(deg*PI / 180.0) + curY * cos(deg*PI / 180.0);
}

bool almost_equal(double x, double y)
{
	return abs(x - y) <= std::numeric_limits<double>::epsilon();
}

void findCoordMaxMin(BoostLineString ls, double &max_x, double &max_y, double &min_x, double &min_y) {
	if (ls.size() == 0) return;

	double x, y;
	getPointData(ls[0], x, y);
	max_x = min_x = x;
	max_y = min_y = y;

	for (int i = 0; i < ls.size(); ++i) {
		getPointData(ls[i], x, y);
		max_x = max(max_x, x), max_y = max(max_y, y);
		min_x = min(min_x, x), min_y = min(min_y, y);
	}
}
