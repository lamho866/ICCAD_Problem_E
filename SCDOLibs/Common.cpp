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
