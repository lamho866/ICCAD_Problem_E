#include "SilkscreenOutput.h"

SilkScreenOutput::SilkScreenOutput(double _silkscreenlen, double _assemblygap, double _cropGap, double _addSafety, BoostPolygon &bgAssembly, vector<Cycle> &cyclePoint, BoostLineString assemblyLs, BoostMultipolygon &cropperMulLsBuffer, BoostMultipolygon &multBGCropper) :
	silkscreenlen(_silkscreenlen), assemblygap(_assemblygap), cropGap(_cropGap), addSafety(_addSafety), bgAssemblyRef(bgAssembly), cyclePt(cyclePoint), cropperMulLsBufferRef(cropperMulLsBuffer), multBGCropperRef(multBGCropper){
	findCoordMaxMin(assemblyLs, as_max_x, as_max_y, as_min_x, as_min_y);
	makeCycleEachPoint(cyclePt, assemblygap, cycleList);
}

void SilkScreenOutput::write(string &fileName) {
	ofstream resultFile(fileName + ".txt");
	for (int i = 0; i < legalSk.size(); ++i) {
		resultFile << "silkscreen\n";
		legalSk[i].write(resultFile);
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

void SilkScreenOutput::ResultOutput(string fileName, Polygom &assembly, BoostMultipolygon &multBGCropper, vector<BoostLineString> &bgDiff) {
	for (int i = 0; i < bgDiff.size(); ++i) {
		if (bg::within(bgDiff[i], multBGCropper) == false && isLargerEnough(bgDiff[i], silkscreenlen)) {
			outputSilkscreen(skSt, bgDiff[i], assembly.cyclePt, cyclePt, cycleList, assemblygap);
		}
	}

	skStCoordSetUp();
	dropLs(canWrite, skSt, assembly.line, assembly.arc);
	classifyLegal();
	
	//skStCoordSafety();
	skStCoordSetUp();
	
	
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

bool SilkScreenOutput::isIlegealAddLine(double x1, double y1, double x2, double y2) {
	Polygom addLsPoly;
	BoostLineString addLs;
	addLsPoly.inputLine(x1, y1, x2, y2);
	makeLine(addLsPoly, addLs);
	//touches: Checks if two geometries have at least one touching point (tangent - non overlapping)
	//intersects: Checks if two geometries have at least one intersection.
	if (bg::intersects(addLs, bgAssemblyRef) || bg::intersects(addLs, multBGCropperRef)) return true;
	if (bg::intersects(addLs, cropperMulLsBufferRef) && !bg::touches(addLs, cropperMulLsBufferRef)) return true;
	return false;

	/*
	return
		(bg::intersects(addLs, cropperMulLsBufferRef) && !bg::touches(addLs, cropperMulLsBufferRef)) ||
		bg::intersects(addLs, bgAssemblyRef) || bg::intersects(addLs, multBGCropperRef);
	*/
}

void SilkScreenOutput::skStCoordSafety() {

	if (skSt_min_x > as_min_x) {
		sort(skSt.begin(), skSt.end(), minXCmp);
		addCoordSafety_X(as_min_x - 0.0001, true);
	}
	
	if (skSt_max_x < as_max_x) {
		sort(skSt.begin(), skSt.end(), maxXCmp);
		addCoordSafety_X(as_max_x + 0.0001, false);
	}
	
	if (skSt_min_y > as_min_y) {
		sort(skSt.begin(), skSt.end(), minYCmp);
		addCoordSafety_Y(as_min_y - 0.0001, true);
	}

	if (skSt_max_y < as_max_y) {
		sort(skSt.begin(), skSt.end(), maxYCmp);
		addCoordSafety_Y(as_max_y + 0.0001, false);
	}
}

void SilkScreenOutput::addCoordSafetyLine(BoostLineString &addLs, vector<BoostLineString> &cropDiff) {
	printf("cropDiff.size(): %d\n", cropDiff.size());
	for (int i = 0; i < cropDiff.size(); ++i) {
		BoostLineString &cropAddLs = cropDiff[i];
		if (bg::intersects(cropAddLs, addLs) && !bg::intersects(cropAddLs, bgAssemblyRef))
		{
			SilkSet sk;
			for (int k = 0; k < cropAddLs.size() - 1; ++k) {
				drawLine(cropAddLs, k, sk);
			}
			skSt.push_back(sk);
			return;
		}
	}
}

void SilkScreenOutput::addCoordSafety_Y(double addedY, bool isLower) {
	canWrite.push_back(true);
	SilkSet sk;
	for (int i = 0; i < skSt.size(); ++i) {
		SilkSet temp = skSt[i];
		Silk head = temp.sk[0];
		Silk tail = temp.sk[temp.sk.size() - 1];
		//insertTail
		if (!isIlegealAddLine(tail.x2, tail.y2, tail.x2, addedY)) {
			//skSt[i].insertLine(static_cast<int>(temp.sk.size()), tail.x2, tail.y2, tail.x2, addedY);
			sk.addLine(tail.x2, tail.y2, tail.x2, addedY);
			skSt.push_back(sk);
			return;
		}
		if (!isIlegealAddLine(head.x1, addedY, head.x1, head.y1)) {
			sk.addLine(head.x1, addedY, head.x1, head.y1);
			skSt.push_back(sk);
			//skSt[i].insertLine(0, head.x1, addedY, head.x1, head.y1);
			return;
		}
	}

	printf("In_Y\n");

	SilkSet &temp = skSt[0];
	Silk &head = temp.sk[0];
	Silk &tail = temp.sk[temp.sk.size() - 1];

	bool isHead = (head.y1 < tail.y2 == isLower);
	Silk &modiftPt = (isHead) ? head : tail;

	BoostPoint bPt1(modiftPt.x1, modiftPt.y1), bPt2(modiftPt.x2, modiftPt.y2);
	BoostLineString addLs{ { modiftPt.x1 , modiftPt.y1 },{ modiftPt.x1, addedY } };

	for (int i = 0; i < multBGCropperRef.size(); ++i) {
		if (bg::intersects(addLs, multBGCropperRef[i])) {
			vector<BoostLineString> cropDiff;
			makeSafetyWithCrop(cropperMulLsBufferRef[i], modiftPt.x1, addedY, cropDiff);
			addCoordSafetyLine(addLs, cropDiff);
			return;
		}
	}
}

void SilkScreenOutput::addCoordSafety_X(double addedX, bool isLower) {
	canWrite.push_back(true);
	SilkSet sk;
	for (int i = 0; i < skSt.size(); ++i) {
		SilkSet temp = skSt[i];
		Silk head = temp.sk[0];
		Silk tail = temp.sk[temp.sk.size() - 1];
		//insertTail
		if (!isIlegealAddLine(tail.x2, tail.y2, addedX, tail.y2)) {
			//skSt[i].insertLine(static_cast<int>(temp.sk.size()), tail.x2, tail.y2, addedX, tail.y2);
			sk.addLine(tail.x2, tail.y2, addedX, tail.y2);
			skSt.push_back(sk);
			return;
		}
		if (!isIlegealAddLine(addedX, head.y1, head.x1, head.y1)) {
			//skSt[i].insertLine(0, addedX, head.y1, head.x1, head.y1);
			sk.addLine(addedX, head.y1, head.x1, head.y1);
			skSt.push_back(sk);
			return;
		}
	}

	printf("In_X\n");

	SilkSet &temp = skSt[0];
	Silk &head = temp.sk[0];
	Silk &tail = temp.sk[temp.sk.size() - 1];

	bool isHead = (head.x1 < tail.x2 == isLower);
	Silk &modiftPt = (isHead) ? head : tail;

	BoostPoint bPt1(modiftPt.x1, modiftPt.y1), bPt2(modiftPt.x2, modiftPt.y2);
	BoostLineString addLs{ { modiftPt.x1 , modiftPt.y1 },{ addedX, modiftPt.y1 } };
	for (int i = 0; i < multBGCropperRef.size(); ++i) {
		if (bg::intersects(addLs, multBGCropperRef[i])) {
			vector<BoostLineString> cropDiff;
			printf("addedX: %lf, modiftPt.y1: %lf\n", addedX, modiftPt.y1);
			makeSafetyWithCrop(cropperMulLsBufferRef[i], addedX, modiftPt.y1, cropDiff);
			addCoordSafetyLine(addLs, cropDiff);
			return;
		}
	}
}

void SilkScreenOutput::classifyLegal() {
	if (canWrite.size() == 0) return;
	double dist;
	for (int i = 0; i < canWrite.size(); ++i) {
		if (!canWrite[i]) continue;
		BoostLineString skLineStr = skSt[i].bgLineStr();
		bool isCropValue = !skCropIsUnValue(skLineStr, multBGCropperRef, cropGap - addSafety, dist);
		bool isAssValue = !skAssIsUnValue(skLineStr, bgAssemblyRef, assemblygap - addSafety, dist);
		if (isCropValue && isAssValue)
			legalSk.push_back(skSt[i]);
		else
			illegalSk.push_back(skSt[i]);
	}
}