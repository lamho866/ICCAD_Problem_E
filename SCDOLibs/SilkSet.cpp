#include "SilkSet.h"

SilkSet::SilkSet() :
	line(0), arc(0), len(0.0), max_x(0.0), max_y(0.0), min_x(0.0), min_y(0.0),
	max_x_cnt(0), max_y_cnt(0), min_x_cnt(0), min_y_cnt(0)
	{}

SilkSet::SilkSet(const SilkSet &skSt) : line(skSt.line), arc(skSt.arc), len(skSt.len)
, max_x(skSt.max_x), max_y(skSt.max_y), min_x(skSt.min_x), min_y(skSt.min_y), sk(skSt.sk) {
	//sk = skSt.sk;
	//sk.resize(skSt.sk.size());
	//copy(skSt.sk.begin(), skSt.sk.end(), sk.begin());
}

SilkSet& SilkSet::operator=(const SilkSet &skSt) {
	line = skSt.line, arc = skSt.arc;
	len = skSt.len;

	max_x = skSt.max_x, max_y = skSt.max_y;
	min_x = skSt.min_x, min_y = skSt.min_y;

	sk.resize(skSt.sk.size());
	copy(skSt.sk.begin(), skSt.sk.end(), sk.begin());

	return *this;
}

bool SilkSet::operator< (const SilkSet &b) const{
	return len < b.len;
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

void SilkSet::updateMinMaxCoord() {
	Polygom poly;
	max_x = min_x = sk[0].x1;
	max_y = min_y = sk[0].y1;

	for (int i = 0; i < sk.size(); ++i) {
		if (sk[i].isLine) poly.inputLine(sk[i].x1, sk[i].y1, sk[i].x2, sk[i].y2);
		else poly.inputArc(sk[i]);
	}

	BoostLineString ls;
	makeLine(poly, ls);
	findCoordMaxMin(ls, max_x, max_y, min_x, min_y);
}

BoostLineString SilkSet::bgLineStr() {
	Polygom bgSkStr;
	BoostLineString lineStr;
	for (int i = 0; i < sk.size(); ++i) {
		Silk &cur = sk[i];
		if (cur.isLine)
			bgSkStr.inputLine(cur.x1, cur.y1, cur.x2, cur.y2);
		else
			bgSkStr.inputArc(cur);
	}
	makeLine(bgSkStr, lineStr);
	return lineStr;
}

bool minXCmp(const SilkSet &a,const SilkSet &b) {
	double aMinX = min(a.sk[0].x1, a.sk[a.sk.size() - 1].x2);
	double bMinX = min(b.sk[0].x1, b.sk[b.sk.size() - 1].x2);
	return aMinX < bMinX;
}
bool minYCmp(const SilkSet &a,const SilkSet &b) {
	double aMinY = min(a.sk[0].y1, a.sk[a.sk.size() - 1].y2);
	double bMinY = min(b.sk[0].y1, b.sk[b.sk.size() - 1].y2);
	return aMinY < bMinY;
}
bool maxXCmp(const SilkSet &a,const  SilkSet &b) {
	double aMaxX = max(a.sk[0].x1, a.sk[a.sk.size() - 1].x2);
	double bMaxX = max(b.sk[0].x1, b.sk[b.sk.size() - 1].x2);
	return aMaxX > bMaxX;
}
bool maxYCmp(const SilkSet &a,const  SilkSet &b) {
	double aMaxY = min(a.sk[0].y1, a.sk[a.sk.size() - 1].y2);
	double bMaxY = min(b.sk[0].y1, b.sk[b.sk.size() - 1].y2);
	return aMaxY > bMaxY;
}
