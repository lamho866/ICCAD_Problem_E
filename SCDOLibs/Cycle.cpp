#include "Cycle.h"

Cycle::Cycle(double _x1, double _y1, double _x2, double _y2, double _rx, double _ry, bool _isCW)
	:x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry), isCW(_isCW) {
	r = dist(x1, y1);
}

double Cycle::dist(double x, double y) {
	return sqrt((x - rx) * (x - rx) + (y - ry) * (y - ry));
}

double Cycle::deg(bool isCW) {
	if (x1 == x2 && y1 == y2)
		return 0.0;

	if (x1 == x2 || y1 == y2)
		return 180.0;

	double va_x = x1 - rx, va_y = y1 - ry;
	double vb_x = x2 - rx, vb_y = y2 - ry;
	double params = va_x * vb_y - va_y * vb_x;

	params /= dist(x1, y1);
	params /= dist(x2, y2);

	double degParams = asin(params) * 180 / PI;
	if (isCW) {
		if (degParams < 0) return -degParams;
		return 180 + degParams;
	}
	//CWW
	if (degParams < 0) return  degParams + 360;
	return degParams;
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
