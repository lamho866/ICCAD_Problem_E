#include "SilkSet.h"

SilkSet::SilkSet() : line(0), arc(0), len(0.0) {}

SilkSet::SilkSet(const SilkSet &skSt) : line(skSt.line), arc(skSt.line), len(skSt.len) {
	sk = skSt.sk;
	//sk.resize(skSt.sk.size());
	//copy(skSt.sk.begin(), skSt.sk.end(), sk.begin());
}

SilkSet& SilkSet::operator=(const SilkSet& skSt) {
	line = skSt.line, arc = skSt.arc;
	len = skSt.len;

	sk.resize(skSt.sk.size());
	copy(skSt.sk.begin(), skSt.sk.end(), sk.begin());

	return *this;
}

void SilkSet::write(ofstream &file) {
	for (int i = 0; i < sk.size(); ++i)
		sk[i].write(file);
}

void SilkSet::addLine(double x1, double y1, double x2, double y2) {
	line++;
	sk.push_back(Silk(true, x1, y1, x2, y2));
	len += sk[sk.size() - 1].len;
}

void SilkSet::addCircle(double x1, double y1, double x2, double y2, double rx, double ry, bool isCW, int cyclePtIdx) {
	arc++;
	sk.push_back(Silk(false, x1, y1, x2, y2, rx, ry, isCW, cyclePtIdx));
	len += sk[sk.size() - 1].len;
}

void SilkSet::insertCircle(int i, double x1, double y1, double x2, double y2, double rx, double ry, bool isCW, int cyclePtIdx) {
	arc++;
	sk.insert(sk.begin() + i, Silk(false, x1, y1, x2, y2, rx, ry, isCW, cyclePtIdx));
	len += sk[i].len;
}

void SilkSet::insertLine(int i, double x1, double y1, double x2, double y2) {
	line++;
	sk.insert(sk.begin() + i, Silk(true, x1, y1, x2, y2));
	len += sk[i].len;
}
