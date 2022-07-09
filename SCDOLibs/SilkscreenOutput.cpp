#include "SilkscreenOutput.h"

SilkScreenOutput::SilkScreenOutput(double _silkscreenlen, double _assemblygap, vector<Cycle> &cyclePt) : silkscreenlen(_silkscreenlen), assemblygap(_assemblygap){
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

void SilkScreenOutput::intputCycle(BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls, int &i) {
	double x1, x2, y1, y2;
	getPointData(ls[i], x1, y1);
	while (i < ls.size() && bg::within(ls[i], cyclePolygon)) ++i;
	getPointData(ls[i - 1], x2, y2);
	--i; // walk back the point, that can make a line, not arc
	skSt[skSt.size() - 1].addCircle(x1, y1, x2, y2, cycle.rx, cycle.ry, cycle.isCW);
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
	int state = 0;
	skSt.push_back(SilkSet());
	int i = 0;
	for (; i < ls.size() - 1; ++i) {
		bool inCycle = false;
		for (int cIdx = 0; cIdx < cycleList.size(); ++cIdx) {
			if (bg::within(ls[i], cycleList[cIdx])) {
				inCycle = true;
				intputCycle(cycleList[cIdx], assemblyCycleList[cIdx], ls, i);
				state = 2;
				break;
			}
		}
		if (!inCycle) {
			drawLine(ls, i);
			state = 1;
		}
	}
}

void SilkScreenOutput::ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff) {
	for (int i = 0; i < bgDiff.size(); ++i) {
		if (bg::within(bgDiff[i], multBGCropper) == false && isLargerEnough(bgDiff[i], silkscreenlen)) {
			outputSilkscreen(bgDiff[i], assembly.cyclePt);
		}
	}
	write(fileName);
}
