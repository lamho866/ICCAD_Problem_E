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
