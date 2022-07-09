#include "Cycle.h"

Cycle::Cycle(double _x1, double _y1, double _x2, double _y2, double _rx, double _ry, bool _isCW)
	:x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry), isCW(_isCW) {
	r = dist(x1, y1);

	stDeg = coordDeg(x1, y1);
	edDeg = coordDeg(x2, y2);
}

double Cycle::dist(double x, double y) {
	return sqrt((x - rx) * (x - rx) + (y - ry) * (y - ry));
}

double Cycle::coordDeg(double x, double y) {
	x -= rx, y -= ry;
	double curDeg = asin(y / r) * 180.0 / PI;
	//sin have 1 and -1
	if (y < 0.0) curDeg = 90.0 - curDeg;
	else curDeg = 90 - curDeg;

	if (x < 0.0) return 360.0 - curDeg;
	return curDeg;
}

double Cycle::deg(bool isCW) {
	stDeg = coordDeg(x1, y1);
	edDeg = coordDeg(x2, y2);

	double curDeg = edDeg - stDeg;
	if (curDeg < 0.0) curDeg += 360.0;

	if (isCW) return curDeg;
	return 360.0 - curDeg;
}

bool Cycle::degInRange(double x, double y) {
	double curDeg = coordDeg(x, y);
	if (stDeg <= edDeg)
		return isCW == (stDeg <= curDeg && curDeg <= edDeg);

	return isCW == (edDeg <= curDeg && curDeg <= edDeg);
}

void Cycle::rotationPt(double curX, double curY, double &nxtX, double &nxtY, bool isCW) {
	curX -= rx;
	curY -= ry;
	//the rotation is around the origin point
	double rDeg = (isCW) ? -rDegSt : rDegSt;
	nxtX = curX * cos(rDeg*PI / 180) - curY * sin(rDeg*PI / 180);
	nxtY = curX * sin(rDeg*PI / 180) + curY * cos(rDeg*PI / 180);

	nxtX += rx;
	nxtY += ry;
}

void Cycle::drawArcycle(const double stX, const double stY, const double endX, const double endY, const double maxDeg, bool isCW, string &shape) {
	double curX = stX, curY = stY;
	double nxtX, nxtY;
	shape += boost::lexical_cast<std::string>(stX) + ' ' + boost::lexical_cast<std::string>(stY) + ',';
	for (double param = rDegSt; param < maxDeg - rDegSt; param += rDegSt) {
		rotationPt(curX, curY, nxtX, nxtY, isCW);
		shape += boost::lexical_cast<std::string>(nxtX) + ' ' + boost::lexical_cast<std::string>(nxtY) + ',';
		curX = nxtX, curY = nxtY;
	}
	shape += boost::lexical_cast<std::string>(endX) + ' ' + boost::lexical_cast<std::string>(endY) + ',';
}
