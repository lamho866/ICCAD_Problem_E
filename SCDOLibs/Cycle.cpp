#include "Cycle.h"

Cycle::Cycle(double _x1, double _y1, double _x2, double _y2, double _rx, double _ry, bool _isCW)
	:x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry), isCW(_isCW), rDegSt(1.0) {
	r = dist(x1, y1);
	
	stDeg = coordDeg(x1, y1);
	edDeg = coordDeg(x2, y2);
}

Cycle::Cycle(const Cycle &c):
x1(c.x1), y1(c.y1), x2(c.x2), y2(c.y2), rx(c.rx), ry(c.ry), isCW(c.isCW), r(c.r), stDeg(c.stDeg), edDeg(c.edDeg), rDegSt(5.0)
{}

Cycle& Cycle::operator=(const Cycle& c) {
	x1 = c.x1, x2 = c.x2;
	y1 = c.y1, y2 = c.y2;
	rx = c.rx, ry = c.ry;
	r = c.r;
	stDeg = c.stDeg, edDeg = c.edDeg;
	isCW = c.isCW;

	return *this;
}

double Cycle::dist(double x, double y) {
	return sqrt((x - rx) * (x - rx) + (y - ry) * (y - ry));
}

double Cycle::coordDeg(double x, double y) {
	x -= rx, y -= ry;
	//OA (0.0, r), OB(x, y)
	double dotProduct = y * r;
	double angle = dotProduct / (r * dist(x + rx, y + ry));
	angle = acos(angle) * 180.0 / PI;
	
	if (x < 0.0) return 360.0 - angle;
	return angle;
}

double Cycle::deg(bool isCW) {
	if (almost_equal(x1, x2) && almost_equal(y1, y2)) return 360.0;
	
	if (isCW) {
		if (edDeg > stDeg) return edDeg - stDeg;
		return 360.0 - (stDeg - edDeg);
	
	}
	if (stDeg > edDeg) return stDeg - edDeg;
	return 360.0 - (edDeg - stDeg);
}

bool Cycle::almost_equal(double x, double y)
{
	return abs(x - y) <= std::numeric_limits<double>::epsilon();
}

bool Cycle::inRange(double deg, double st, double ed) {
	return (almost_equal(st, deg) || st < deg) && (deg < ed || almost_equal(deg, ed));
}

bool Cycle::degInRange(double x, double y) {
	double curDeg = coordDeg(x, y);
	if (stDeg <= edDeg)
		return isCW == inRange(curDeg, stDeg, edDeg);

	return !isCW == inRange(curDeg, edDeg, stDeg);
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

void Cycle::translatePt(double targetR, double &x, double &y) {
	double curR = dist(x, y);
	double unitVx = (x - rx) / curR, unitVy = (y - ry) / curR;
	x = unitVx * targetR + rx;
	y = unitVy * targetR + ry;
}
