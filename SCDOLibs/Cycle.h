#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <math.h>
#include <boost/lexical_cast.hpp>
using namespace std;

#define PI 3.14159265
class Cycle
{
private:
	double dist(double x, double y);
	bool almost_equal(double x, double y);
	bool inRange(double deg, double st, double ed);
public: //CW or CCW 
	const double rDegSt = 5.0;
	double x1, y1, x2, y2, rx, ry, r;
	double stDeg, edDeg;
	bool isCW;
	Cycle(double _x1, double _y1, double _x2, double _y2, double _rx, double _ry, bool _isCW);

	double coordDeg(double x, double y);

	double deg(bool isCW);

	bool degInRange(double ptX, double ptY);

	void rotationPt(double curX, double curY, double &nxtX, double &nxtY, bool isCW);

	void drawArcycle(const double stX, const double stY, const double endX, const double endY, const double maxDeg, bool isCW, string &shape);
};
