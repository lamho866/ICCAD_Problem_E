#include "SilkSet.h"

SilkSet::SilkSet() : line(0), arc(0), len(0.0) {}

void SilkSet::write(ofstream &file) {
	for (int i = 0; i < sk.size(); ++i)
		sk[i].write(file);
}

void SilkSet::addLine(double &x1, double &y1, double &x2, double &y2) {
	sk.push_back(Silk(true, x1, y1, x2, y2));
	len += sk[sk.size() - 1].len;
}

void SilkSet::addCircle(double &x1, double &y1, double &x2, double &y2, double &rx, double &ry, bool &isCW) {
	sk.push_back(Silk(false, x1, y1, x2, y2, rx, ry, isCW));
	len += sk[sk.size() - 1].len;
}
