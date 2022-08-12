#include "OutputSilkscreen.h"

void outputSilkscreen(vector<SilkSet> &skSt, BoostLineString &ls, vector<Cycle> assemblyCycleList, vector<Cycle> &cyclePt, vector<BoostPolygon> &cycleList) {
	SilkSet sk;
	double x, y, nxtX, nxtY;
	int i = 0;
	int cIdx = 0, cSize = static_cast<int>(cyclePt.size());
	
	for (; i < ls.size() - 1; ++i) {
		getPointData(ls[i], x, y);
		getPointData(ls[i + 1], nxtX, nxtY);
		bool inCycle = false;
		for (int j = 0; j < cSize; ++j, cIdx = (cIdx + 1) % cSize) {
			if (bg::within(ls[i], cycleList[cIdx]) && cyclePt[cIdx].degInRange(x, y) &&
				bg::within(ls[i + 1], cycleList[cIdx]) && cyclePt[cIdx].degInRange(nxtX, nxtY)) {
				intputCycle(cycleList[cIdx], assemblyCycleList[cIdx], ls, cIdx, i, sk);
				cIdx = (cIdx + 1) % cSize;
				inCycle = true;
				--i;
				break;
			}
		}

		if (i < ls.size() - 1 && !inCycle)
			drawLine(ls, i, sk);
	}

	arcCombine(sk);
	skSt.push_back(sk);
}

void intputCycle(BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls, int cIdx, int &i, SilkSet &sk) {
	double x1, x2, y1, y2;
	getPointData(ls[i], x1, y1);
	double x = x1, y = y1;
	double Rstd = cycle.dist(x, y);
	while (i < ls.size() && bg::within(ls[i], cyclePolygon)) {
		getPointData(ls[i], x, y);
		++i;
		double curR = cycle.dist(x, y);
		if (!(Rstd - 0.0001 <= curR && curR <= Rstd + 0.0001)) break;
	}
	getPointData(ls[i - 1], x2, y2);
	--i; // walk back the point, that can make a line, not arc
	sk.addCircle(x1, y1, x2, y2, cycle.rx, cycle.ry, cycle.isCW, cIdx);
}

bool collinear(BoostPoint pt1, BoostPoint pt2, BoostPoint pt3)
{
	double x1, x2, x3, y1, y2, y3;
	getPointData(pt1, x1, y1);
	getPointData(pt2, x2, y2);
	getPointData(pt3, x3, y3);
	double a = x1 * (y2 - y3) +
		x2 * (y3 - y1) +
		x3 * (y1 - y2);

	return abs(a) <= std::numeric_limits<double>::epsilon();
}

void drawLine(BoostLineString &ls, int &i, SilkSet &sk) {
	double x1, x2, y1, y2;
	getPointData(ls[i], x1, y1);
	int j = i + 2;
	for (; j < ls.size(); ++j)
		if (!collinear(ls[i], ls[i + 1], ls[j]))
			break;
	getPointData(ls[j - 1], x2, y2);
	i = j - 2;
	sk.addLine(x1, y1, x2, y2);
}

void arcCombine(SilkSet &skLs) {
	for (int i = 0; i < skLs.sk.size() - 1; ++i) {
		if (!skLs.sk[i].isLine && !skLs.sk[i].isLine &&
			almost_equal(skLs.sk[i].rx, skLs.sk[i + 1].rx) && almost_equal(skLs.sk[i].ry, skLs.sk[i + 1].ry) &&
			almost_equal(skLs.sk[i].x2, skLs.sk[i + 1].x1) && almost_equal(skLs.sk[i].y2, skLs.sk[i + 1].y1)) {
			skLs.sk[i].x2 = skLs.sk[i + 1].x2;
			skLs.sk[i].y2 = skLs.sk[i + 1].y2;

			skLs.sk.erase(skLs.sk.begin() + i + 1);
			--i;
		}
	}
}