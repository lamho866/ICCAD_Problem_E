#include "Polgyom.h"

void Polygom::addLine(string &s) {
	char x1[10], x2[10], rx[10], y1[10], y2[10], ry[10], tap[10];
	for (int i = 0; i < s.size(); ++i)
		if (s[i] == ',')
			s[i] = ' ';
	if (s[0] == 'l') {
		//isLine
		sscanf(s.c_str() + 5, "%s %s %s %s\n", x1, y1, x2, y2);
		shape += boost::lexical_cast<std::string>(x1) + ' ' + boost::lexical_cast<std::string>(y1) + ',' + boost::lexical_cast<std::string>(x2) + ' ' + boost::lexical_cast<std::string>(y2) + ',';
	}
	if (s[0] == 'a') {
		for (int i = 0; i < s.size(); ++i)
			if (s[i] == ',')
				s[i] = ' ';
		sscanf(s.c_str() + 4, "%s %s %s %s %s %s %s", x1, y1, x2, y2, rx, ry, tap);
		double curX, curY, nxtX, nxtY;
		bool isCW = strcmp(tap, "CW") == 0;

		Cycle c(atof(x1), atof(y1), atof(x2), atof(y2), atof(rx), atof(ry), isCW);
		cyclePt.push_back(c);
		double maxDeg = c.deg(isCW);

		c.drawArcycle(c.x1, c.y1, c.x2, c.y2, maxDeg, isCW, shape);
	}

}

