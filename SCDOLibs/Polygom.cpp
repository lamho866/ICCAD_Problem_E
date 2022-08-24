#include "Polygom.h"

Polygom::Polygom() : line(0), arc(0) {}

void Polygom::inputLine(double &x1, double &y1, double &x2, double &y2) {
	++line;
	shape += boost::lexical_cast<std::string>(x1) + ' ' + boost::lexical_cast<std::string>(y1) + ',' + boost::lexical_cast<std::string>(x2) + ' ' + boost::lexical_cast<std::string>(y2) + ',';
}

void Polygom::inputArc(Cycle &c) {
	++arc;
	cyclePt.push_back(c);
	double maxDeg = c.deg(c.isCW);
	c.drawArcycle(c.x1, c.y1, c.x2, c.y2, maxDeg, c.isCW, shape);
}

void Polygom::addLine(string &s) {
	char x1[10], x2[10], rx[10], y1[10], y2[10], ry[10], tap[10];
	for (int i = 0; i < s.size(); ++i)
		if (s[i] == ',')
			s[i] = ' ';
	if (s[0] == 'l') {
		//isLine
		++line;
		sscanf(s.c_str() + 5, "%s %s %s %s\n", x1, y1, x2, y2);
		shape += boost::lexical_cast<std::string>(x1) + ' ' + boost::lexical_cast<std::string>(y1) + ',' + boost::lexical_cast<std::string>(x2) + ' ' + boost::lexical_cast<std::string>(y2) + ',';
		return;
	}
	if (s[0] == 'a') {
		for (int i = 0; i < s.size(); ++i)
			if (s[i] == ',')
				s[i] = ' ';
		sscanf(s.c_str() + 4, "%s %s %s %s %s %s %s", x1, y1, x2, y2, rx, ry, tap);
		double curX, curY, nxtX, nxtY;
		bool isCW = strcmp(tap, "CW") == 0;

		Cycle c(atof(x1), atof(y1), atof(x2), atof(y2), atof(rx), atof(ry), isCW);
		inputArc(c);
	}

}

bool Polygom::isNear(double x1, double y1, double x2, double y2) {
	double dist = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	return dist <= 0.0002;
}

bool Polygom::isConnect(Cycle a, Cycle b) {
	return a.almost_equal(a.x2, b.x1) && a.almost_equal(a.y2, b.y1);
}

void Polygom::cyclePtCombe() {

	for (int i = 0; i < static_cast<int>(cyclePt.size()) - 1; ++i) {
		if (isNear(cyclePt[i].rx, cyclePt[i].ry, cyclePt[i + 1].rx, cyclePt[i + 1].ry) && isConnect(cyclePt[i], cyclePt[i + 1]))
		{
			cyclePt[i].x2 = cyclePt[i + 1].x2;
			cyclePt[i].y2 = cyclePt[i + 1].y2;
			cyclePt[i].edDeg = cyclePt[i + 1].edDeg;
			cyclePt.erase(cyclePt.begin() + i + 1);
			--i;
		}
	}
}
