#include "SilkscreenOutput.h"

SilkScreenOutput::SilkScreenOutput(double _silkscreenlen, double _assemblygap, vector<Cycle> &cyclePoint, BoostLineString assemblyLs) : silkscreenlen(_silkscreenlen), assemblygap(_assemblygap), cyclePt(cyclePoint) {
	findCoordMaxMin(assemblyLs, as_max_x, as_max_y, as_min_x, as_min_y);
	makeCycleEachPoint(cyclePt, assemblygap, cycleList);
}

void SilkScreenOutput::write(string &fileName) {
	if (canWrite.size() != skSt.size()) {
		canWrite.resize(skSt.size());
		fill(canWrite.begin(), canWrite.end(), true);
	}

	ofstream resultFile(fileName + ".txt");
	for (int i = 0; i < skSt.size(); ++i) {
		if (canWrite[i]) {
			resultFile << "silkscreen\n";
			skSt[i].write(resultFile);
		}
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

void SilkScreenOutput::intputCycle(BoostPolygon cyclePolygon, Cycle cycle, BoostLineString &ls, int cIdx, int &i) {
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

	return abs(a) <= std::numeric_limits<double>::epsilon();
}

void SilkScreenOutput::drawLine(BoostLineString &ls, int &i) {
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
		if (i < ls.size() - 1)
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
	//silkScreenModify();
	
	skStCoordSetUp();
	/*
	printf("\n----------------------\n");
	printf("assembly:\n");
	printf("maxX: %lf, maxY: %lf, minX: %lf, minY: %lf\n", as_max_x, as_max_y, as_min_x, as_min_y);
	printf("SilkSet:\n");
	printf("maxX: %lf, maxY: %lf, minX: %lf, minY: %lf\n", skSt_max_x, skSt_max_y, skSt_min_x, skSt_min_y);
	printf("\n----------------------\n");
	*/
	silkCoordMinXSafety();
	silkCoordMaxXSafety();
	silkCoordMinYSafety();
	silkCoordMaxYSafety();

	skStCoordSetUp();
	//dropLs();
	write(fileName);
}

void SilkScreenOutput::skStCoordSetUp() {
	if (skSt.size() == 0) return;

	skSt[0].updateMinMaxCoord();
	skSt_max_x = skSt[0].max_x, skSt_min_x = skSt[0].min_x;
	skSt_max_y = skSt[0].max_y, skSt_min_y = skSt[0].min_y;

	for (int i = 1; i < skSt.size(); ++i) {
		skSt[i].updateMinMaxCoord();
		skSt_max_x = max(skSt_max_x, skSt[i].max_x), skSt_min_x = min(skSt_min_x, skSt[i].min_x);
		skSt_max_y = max(skSt_max_y, skSt[i].max_y), skSt_min_y = min(skSt_min_y, skSt[i].min_y);
	}
}

void SilkScreenOutput::isCoordEquals(double max_cr, double cur_max, int &cnt) {
	if (almost_equal(max_cr, cur_max)) cnt++;
}

void SilkScreenOutput::cntMaxMin(double max_x, double max_y, double min_x, double min_y, int &max_x_cnt, int &max_y_cnt, int &min_x_cnt, int &min_y_cnt, SilkSet &skSt) {
	isCoordEquals(max_x, skSt.max_x, max_x_cnt);
	isCoordEquals(max_y, skSt.max_y, max_y_cnt);
	isCoordEquals(min_x, skSt.min_x, min_x_cnt);
	isCoordEquals(min_y, skSt.min_y, min_y_cnt);
}

void SilkScreenOutput::dropLs() {
	canWrite.resize(skSt.size());
	fill(canWrite.begin(), canWrite.end(), true);

	double max_x = -9999.0, min_x = 9999.0, max_y = -9999.0, min_y = 9999.0;


	for (int i = 0; i < skSt.size(); ++i) {
		max_x = max(max_x, skSt[i].max_x), max_y = max(max_y, skSt[i].max_y);
		min_x = min(min_x, skSt[i].min_x), min_y = min(min_y, skSt[i].min_y);
	}

	int max_x_cnt = 0, max_y_cnt = 0, min_x_cnt = 0, min_y_cnt = 0;
	for (int i = 0; i < skSt.size(); ++i) {
		cntMaxMin(max_x, max_y, min_x, min_y, max_x_cnt, max_y_cnt, min_x_cnt, min_y_cnt, skSt[i]);
	}

	sort(skSt.begin(), skSt.end());

	for (int i = 0; i < skSt.size(); ++i) {
		int cur_max_x = 0, cur_max_y = 0, cur_min_x = 0, cur_min_y = 0;
		cntMaxMin(max_x, max_y, min_x, min_y, cur_max_x, cur_max_y, cur_min_x, cur_min_y, skSt[i]);
	
		if (max_x_cnt - cur_max_x > 0 && 
			max_y_cnt - cur_max_y > 0 && 
			min_x_cnt - cur_min_x > 0 && 
			min_y_cnt - cur_min_y > 0) {
			
			max_x_cnt -= cur_max_x;
			max_y_cnt -= cur_max_y;
			min_x_cnt -= cur_min_x;
			min_y_cnt -= cur_min_y;
			canWrite[i] = false;
		}
	}
}

void SilkScreenOutput::silkCoordMinXSafety() {
	if (skSt_min_x < as_min_x) return;
	sort(skSt.begin(), skSt.end(), minXCmp);
	for (int i = 0; i < skSt.size(); ++i) {
		SilkSet temp = skSt[i];
		Silk head = temp.sk[0];
		Silk tail = temp.sk[temp.sk.size() - 1];
		if (head.x1 < tail.x1) {
			//insertHead
			skSt[i].insertLine(0, as_min_x, head.y1, head.x1, head.y1);
			//notCover the Cropper
			return;
		}
		else {
			//insertTail
			skSt[i].insertLine(temp.sk.size(), tail.x2, tail.y2, as_min_x, tail.y2);
			//notCover the Cropper
			return;
		}
	}
}

void SilkScreenOutput::silkCoordMaxXSafety() {
	if (skSt_max_x > as_max_x) return;
	sort(skSt.begin(), skSt.end(), maxXCmp);

	for (int i = 0; i < skSt.size(); ++i) {
		SilkSet temp = skSt[i];
		Silk head = temp.sk[0];
		Silk tail = temp.sk[temp.sk.size() - 1];
		if (head.x1 < tail.x1) {
			//insertHead
			skSt[i].insertLine(0, as_max_x, head.y1, head.x1, head.y1);
			//notCover the Cropper
			return;
		}
		else {
			//insertTail
			skSt[i].insertLine(temp.sk.size(), tail.x2, tail.y2, as_max_x, tail.y2);
			//notCover the Cropper
			return;
		}
	}
}

void SilkScreenOutput::silkCoordMinYSafety() {
	if (skSt_min_y < as_min_y) return;
	sort(skSt.begin(), skSt.end(), minYCmp);

	for (int i = 0; i < skSt.size(); ++i) {
		SilkSet temp = skSt[i];
		Silk head = temp.sk[0];
		Silk tail = temp.sk[temp.sk.size() - 1];
		if (head.y1 < tail.y2) {
			//insertHead
			skSt[i].insertLine(0, head.x1, as_min_y, head.x1, head.y1);
			//notCover the Cropper
			return;
		}
		else {
			//insertTail
			skSt[i].insertLine(temp.sk.size(), tail.x2, tail.y2, tail.x2, as_min_y);
			//notCover the Cropper
			return;
		}
	}
}

void SilkScreenOutput::silkCoordMaxYSafety() {
	if (skSt_max_y > as_max_y) return;
	sort(skSt.begin(), skSt.end(), maxYCmp);

	for (int i = 0; i < skSt.size(); ++i) {
		SilkSet temp = skSt[i];
		Silk head = temp.sk[0];
		Silk tail = temp.sk[temp.sk.size() - 1];
		if (head.y1 > tail.y2) {
			//insertHead
			skSt[i].insertLine(0, head.x1, as_max_y, head.x1, head.y1);
			//notCover the Cropper
			return;
		}
		else {
			//insertTail
			skSt[i].insertLine(temp.sk.size(), tail.x2, tail.y2, tail.x2, as_max_y);
			//notCover the Cropper
			return;
		}
	}
}
