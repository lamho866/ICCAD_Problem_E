#include "SilkscreenOutput.h"

SilkScreenOutput::SilkScreenOutput(double _silkscreenlen, double _assemblygap, vector<Cycle> &cyclePoint) : silkscreenlen(_silkscreenlen), assemblygap(_assemblygap), cyclePt(cyclePoint){
	makeCycleEachPoint(cyclePt, assemblygap, cycleList);
}

void SilkScreenOutput::write(string &fileName) {
	ofstream resultFile(fileName + ".txt");
	for (int i = 0; i < skSt.size(); ++i) {
		resultFile << "silkscreen\n";
		skSt[i].write(resultFile);
	}
	resultFile.close();
}

void SilkScreenOutput::makeCycleEachPoint(vector<Cycle> &cyclePt, const double assemblyGap, vector<BoostPolygon> &cycleList) {
	double addR = assemblyGap * 1.5;
	for (int i = 0; i < cyclePt.size(); ++i) {
		string shape = "";
		BoostPolygon cycleShape;
		Cycle c = cyclePt[i];
		double x = c.rx + c.r;
		double y = c.ry;

		if (c.isCW)x += addR;
		c.drawArcycle(x, y, x, y, 360, true, shape);
		makeThePolygonShape(shape, cycleShape);
		cycleList.push_back(cycleShape);
	}
}

void SilkScreenOutput::intputCycle(BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls,int cIdx, int &i) {
	double x1, x2, y1, y2;
	getPointData(ls[i], x1, y1);
	double x = x1, y = y1;
	while (i < ls.size() && bg::within(ls[i], cyclePolygon)) {
		getPointData(ls[i], x, y);
		++i;
	}
	getPointData(ls[i - 1], x2, y2);
	--i; // walk back the point, that can make a line, not arc
	skSt[skSt.size() - 1].addCircle(x1, y1, x2, y2, cycle.rx, cycle.ry, cycle.isCW, cIdx);
}

bool SilkScreenOutput::collinear(BoostPoint pt1, BoostPoint pt2, BoostPoint pt3)
{
	double x1, x2, x3, y1, y2, y3;
	getPointData(pt1, x1, y1);
	getPointData(pt2, x2, y2);
	getPointData(pt3, x3, y3);
	double a = x1 * (y2 - y3) +
		x2 * (y3 - y1) +
		x3 * (y1 - y2);

	return a == 0.0;
}

void SilkScreenOutput::drawLine(BoostLineString &ls, int &i){
	double x1, x2, y1, y2;
	getPointData(ls[i], x1, y1);
	int j = i + 2;
	for (; j < ls.size(); ++j)
		if (!collinear(ls[i], ls[i + 1], ls[j]))
			break;
	getPointData(ls[j - 1], x2, y2);
	i = j - 2;
	skSt[skSt.size() - 1].addLine(x1, y1, x2, y2);
}

void SilkScreenOutput::outputSilkscreen(BoostLineString &ls, vector<Cycle> assemblyCycleList) {
	skSt.push_back(SilkSet());
	double x, y;
	int i = 0;
	for (; i < ls.size() - 1; ++i) {
		getPointData(ls[i], x, y);
		for (int cIdx = 0; cIdx < cycleList.size(); ++cIdx) {
			if (bg::within(ls[i], cycleList[cIdx]) && cyclePt[cIdx].degInRange(x, y)) {
				intputCycle(cycleList[cIdx], assemblyCycleList[cIdx], ls, cIdx, i);
				break;
			}
		}
		if(i < ls.size() - 1)
			drawLine(ls, i);
	}
}

void SilkScreenOutput::addTurningPoint(int &i, SilkSet &skSet, bool isHeaed) {
	Silk &sk = skSet.sk[i];
	Cycle c = cyclePt[sk.cyclePtIdx];
	double r = c.r - assemblygap;

	if (isHeaed) {
		double x1, y1;
		rtPt(0.0, r, -c.stDeg, x1, y1);
		x1 += c.rx, y1 += c.ry;
		sk.x1 = x1, sk.y1 = y1;

		skSet.insertLine(i, skSet.sk[i - 1].x2, y1, x1, y1);
		skSet.insertLine(i, skSet.sk[i - 1].x2, skSet.sk[i - 1].y2, skSet.sk[i - 1].x2, y1);
		return;
	}

	double x2, y2;
	rtPt(0.0, r, -c.edDeg, x2, y2);
	x2 += c.rx, y2 += c.ry;
	sk.x2 = x2, sk.y2 = y2;

	skSet.insertLine(i + 1, x2, y2, skSet.sk[i + 1].x1, y2);
	skSet.insertLine(i + 1, skSet.sk[i + 1].x1, y2, skSet.sk[i + 1].x1, skSet.sk[i + 1].y1);
}

void SilkScreenOutput::addArcSafetyLine(int &i, SilkSet &skSet) {
	if (i > 0 && skSet.sk[i - 1].isLine) {
		addTurningPoint(i, skSet, true);
		//back to arc
		i += 2;
	}

	if (i + 1 < skSet.sk.size() && skSet.sk[i + 1].isLine) {
		addTurningPoint(i, skSet, false);
		//move out arc & addTurningPoint
		i += 3;
	}
}

void SilkScreenOutput::arcLineCheck(SilkSet &skSet) {
	for (int i = 0; i < skSet.sk.size(); ++i) {
		if (skSet.sk[i].isLine == false && skSet.sk[i].isCW == false) {
			addArcSafetyLine(i, skSet);
		}
	}
}

void SilkScreenOutput::silkScreenModify() {
	for (int i = 0; i < skSt.size(); ++i) {
		arcLineCheck(skSt[i]);
	}
}

void SilkScreenOutput::ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff) {
	for (int i = 0; i < bgDiff.size(); ++i) {
		if (bg::within(bgDiff[i], multBGCropper) == false && isLargerEnough(bgDiff[i], silkscreenlen)) {
			outputSilkscreen(bgDiff[i], assembly.cyclePt);
		}
	}
	silkScreenModify();
	write(fileName);
}
