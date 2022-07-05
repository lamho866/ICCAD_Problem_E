#include "SilkscreenOutput.h"

void makeCycleEachPoint(vector<Cycle> &cyclePt, const double assemblyGap, vector<BoostPolygon> &cycleList) {
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

bool isLargerEnough(BoostLineString silkScreen, const double silkscreenlen) {
	//cout << "Len : " << bg::length(silkScreen) << endl;
	return (bg::length(silkScreen)) > silkscreenlen;
}

void inputPoint(ofstream &file, BoostPoint &pt) {
	file << ',' << fixed << setprecision(4) << bg::get<0>(pt) << ',' << fixed << setprecision(4) << bg::get<1>(pt);
}

void inputPoint(ofstream &file, const double x, const double y) {
	file << ',' << fixed << setprecision(4) << x << ',' << fixed << setprecision(4) << y;
}


void intputCycle(ofstream &file, BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls, int &i) {
	file << "arc";
	inputPoint(file, ls[i]);
	while (i < ls.size() && bg::within(ls[i], cyclePolygon)) ++i;
	inputPoint(file, ls[i - 1]);
	--i; // walk back the point, that can make a line, not arc
	inputPoint(file, cycle.rx, cycle.ry);
	if (cycle.isCW)
		file << ",CW";
	else
		file << ",CCW";
	file << std::endl;
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

	return a == 0.0;
}

void drawLine(ofstream &file, BoostLineString &ls, int &i) {
	file << "line";
	inputPoint(file, ls[i]);
	int j = i + 2;
	for (; j < ls.size(); ++j)
		if (!collinear(ls[i], ls[i + 1], ls[j]))
			break;
	inputPoint(file, ls[j - 1]);
	i = j - 2;
	file << std::endl;
}

void outputSilkscreen(ofstream &file, BoostLineString &ls, vector<BoostPolygon> &cyclePolygonList, vector<Cycle> cycleList) {
	file << "silkscreen\n";
	int i = 0;
	for (; i < ls.size() - 1; ++i) {
		bool inCycle = false;
		for (int cIdx = 0; cIdx < cyclePolygonList.size(); ++cIdx) {
			if (bg::within(ls[i], cyclePolygonList[cIdx])) {
				inCycle = true;
				intputCycle(file, cyclePolygonList[cIdx], cycleList[cIdx], ls, i);
				break;
			}
		}
		if (!inCycle)drawLine(file, ls, i);
	}
}

void ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff, vector<BoostPolygon> &cycleList, double silkscreenlen) {
	ofstream resultFile(fileName);
	for (int i = 0; i < bgDiff.size(); ++i) {
		if (bg::within(bgDiff[i], multBGCropper) == false && isLargerEnough(bgDiff[i], silkscreenlen)) {
			outputSilkscreen(resultFile, bgDiff[i], cycleList, assembly.cyclePt);
		}
	}
	resultFile.close();
}