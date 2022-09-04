#include "SilkscreenOutput.h"

SilkScreenOutput::SilkScreenOutput(double _silkscreenlen, double _assemblygap, double _cropGap, BoostPolygon &bgAssembly, vector<Cycle> &cyclePoint, BoostLineString assemblyLs, BoostMultipolygon &cropperMulLsBuffer, BoostMultipolygon &multBGCropper) :
	silkscreenlen(_silkscreenlen), assemblygap(_assemblygap), cropGap(_cropGap), bgAssemblyRef(bgAssembly), cyclePt(cyclePoint), cropperMulLsBufferRef(cropperMulLsBuffer), multBGCropperRef(multBGCropper){
	findCoordMaxMin(assemblyLs, as_max_x, as_max_y, as_min_x, as_min_y);
	makeCycleEachPoint(cyclePt, assemblygap, cycleList);
	rAdoptRange = max(_assemblygap / 100.0, 0.0001);
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
			outputSilkscreen(skSt, bgDiff[i], assembly.cyclePt, cyclePt, cycleList, assemblygap, rAdoptRange);
		}
	}

	skStCoordSetUp(skSt);
	dropLs(canWrite, skSt, assembly.line, assembly.arc);
	classifyLegal();
}

void SilkScreenOutput::skStCoordSetUp(vector<SilkSet> &skStcur) {
	if (skStcur.size() == 0) return;

	skStcur[0].updateMinMaxCoord();
	skSt_max_x = skStcur[0].max_x, skSt_min_x = skStcur[0].min_x;
	skSt_max_y = skStcur[0].max_y, skSt_min_y = skStcur[0].min_y;

	for (int i = 1; i < skStcur.size(); ++i) {
		skStcur[i].updateMinMaxCoord();
		skSt_max_x = max(skSt_max_x, skStcur[i].max_x), skSt_min_x = min(skSt_min_x, skStcur[i].min_x);
		skSt_max_y = max(skSt_max_y, skStcur[i].max_y), skSt_min_y = min(skSt_min_y, skStcur[i].min_y);
	}
}

bool SilkScreenOutput::isIlegealAddLine(double x1, double y1, double x2, double y2) {
	Polygom addLsPoly;
	BoostLineString addLs;
	addLsPoly.inputLine(x1, y1, x2, y2);
	makeLine(addLsPoly, addLs);
	//touches: Checks if two geometries have at least one touching point (tangent - non overlapping)
	//intersects: Checks if two geometries have at least one intersection.
	/*
	printf("try add line (%lf, %lf, %lf, %lf)\n", x1, y1, x2, y2);
	printf("cond1 bg::intersects(addLs, bgAssemblyRef): %d\n", bg::intersects(addLs, bgAssemblyRef));
	printf("cond2 bg::intersects(addLs, multBGCropperRef): %d\n", bg::intersects(addLs, multBGCropperRef));
	printf("cond3 bg::intersects(addLs, cropperMulLsBufferRef): %d\n", bg::intersects(addLs, cropperMulLsBufferRef));
	printf("cond4 !bg::touches(addLs, cropperMulLsBufferRef): %d\n\n", !bg::touches(addLs, cropperMulLsBufferRef));
	*/
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
	//printf("Stop\n");
	skStCoordSetUp(legalSk);

	if (skSt_min_x > as_min_x) {
		//printf("modify mix_x =====>\n");
		sort(legalSk.begin(), legalSk.end(), minXCmp);
		addCoordSafety(as_min_x - 0.0001, true, true);
	}
	
	if (skSt_max_x < as_max_x) {
		//printf("modify max_x =====>\n");
		sort(legalSk.begin(), legalSk.end(), maxXCmp);
		addCoordSafety(as_max_x + 0.0001, false, true);
	}
	
	if (skSt_min_y > as_min_y) {
		//printf("modify min_y =====>\n");
		sort(legalSk.begin(), legalSk.end(), minYCmp);
		addCoordSafety(as_min_y - 0.0001, true, false);
	}

	if (skSt_max_y < as_max_y) {
		//printf("modify max_y =====>\n");
		sort(legalSk.begin(), legalSk.end(), maxYCmp);
		addCoordSafety(as_max_y + 0.0001, false, false);
	}
}

void SilkScreenOutput::addCoordSafetyLine(BoostLineString &addLs, vector<BoostLineString> &cropDiff, SilkSet &sk) {
	//printf("cropDiff.size(): %d\n", cropDiff.size());
	for (int i = 0; i < cropDiff.size(); ++i) {
		BoostLineString &cropAddLs = cropDiff[i];
		//printf("[i] => %d, %d\n", bg::intersects(cropAddLs, addLs) ,!bg::intersects(cropAddLs, bgAssemblyRef));
		if (bg::intersects(cropAddLs, addLs) && !bg::intersects(cropAddLs, bgAssemblyRef))
		{
			for (int k = 0; k < cropAddLs.size() - 1; ++k) {
				drawLine(cropAddLs, k, sk);
			}
			return;
		}
	}
}

bool SilkScreenOutput::canAddstraightLine(double x1, double y1, double x2, double y2, SilkSet &sk){
	
	BoostLineString addLs{ {x1, y1}, {x2, y2} };
	if (!isIlegealAddLine(x1, y1, x2, y2) && lineIsLegal(addLs)) {
		sk.addLine(x1, y1, x2, y2);
		return true;
	}
	return false;
}

bool SilkScreenOutput::canAddAroundCrop(SilkSet &curSkst, double added, bool isLower, bool isX, SilkSet &sk) {
	Silk &head = curSkst.sk[0];
	Silk &tail = curSkst.sk[curSkst.sk.size() - 1];

	BoostPoint ptCheck;
	double ptX, ptY;
	BoostLineString addLs;

	if (isX) {
		bool isHead = (head.x1 < tail.x2 == isLower);
		Silk &modiftPt = (isHead) ? head : tail;

		if ((modiftPt.x1 < modiftPt.x2) == isLower) ptX = modiftPt.x1, ptY = modiftPt.y1;
		else ptX = modiftPt.x2, ptY = modiftPt.y2;
		ptCheck = BoostPoint(added, ptY);

		if (isLower)
			addLs = BoostLineString{ { ptX + 0.0001, ptY },{ added, ptY } };
		else
			addLs = BoostLineString{ { ptX - 0.0001, ptY },{ added, ptY } };
	}
	else {
		bool isHead = (head.y1 < tail.y2 == isLower);
		Silk &modiftPt = (isHead) ? head : tail;

		if ((modiftPt.y1 < modiftPt.y2) == isLower) ptX = modiftPt.x1, ptY = modiftPt.y1;
		else ptX = modiftPt.x2, ptY = modiftPt.y2;
		ptCheck = BoostPoint(ptX, added);
		if (isLower)
			addLs = BoostLineString{ { ptX, ptY + 0.0001 },{ ptX, added } };
		else
			addLs = BoostLineString{ { ptX, ptY - 0.0001 },{ ptX, added } };
	}
	
	for (int i = 0; i < cropperMulLsBufferRef.size(); ++i) {
		//printf("[%d] ==> (%d, %d, %d)\n",i ,bg::intersects(addLs, cropperMulLsBufferRef[i]), !bg::touches(addLs, cropperMulLsBufferRef[i]), bg::covered_by(ptCheck, cropperMulLsBufferRef[i].outer()));
		if (bg::intersects(addLs, cropperMulLsBufferRef[i]) && 
			!bg::touches(addLs, cropperMulLsBufferRef[i]) && 
			bg::covered_by(ptCheck, cropperMulLsBufferRef[i].outer())
			) {
			vector<BoostLineString> cropDiff;
			//printf("IN\n");
			if(isX) makeSafetyWithCrop(cropperMulLsBufferRef[i], added, ptY, cropDiff);
			else makeSafetyWithCrop(cropperMulLsBufferRef[i], ptX, added, cropDiff);
			if (cropDiff.size() == 0) continue;

			addCoordSafetyLine(addLs, cropDiff, sk);
			if(sk.sk.size() != 0)
				return true;
		}
	}
	//printf("Using final addCropper way!!\n");
	int minIdx = 0;
	double minDist = bg::distance(ptCheck, cropperMulLsBufferRef[0]);
	for (int i = 1; i < cropperMulLsBufferRef.size(); ++i) {
		double curDist = bg::distance(ptCheck, cropperMulLsBufferRef[i]);
		if (curDist < minDist) {
			minDist = curDist;
			minIdx = i;
		}
	}

	vector<BoostLineString> cropDiff;
	if (isX) {
		if (isLower) ptX -= 0.0001;
		else ptX += 0.0001;

		specialCutWithCrop(cropperMulLsBufferRef[minIdx], added, ptX, isX, cropDiff);
		addLs = BoostLineString{ { added, as_max_y },{ added, as_min_y } };
	}
	else {
		if (isLower) ptY -= 0.0001;
		else ptY += 0.0001;

		specialCutWithCrop(cropperMulLsBufferRef[minIdx], added, ptY, isX, cropDiff);
		addLs = BoostLineString{ { as_min_x, added },{ as_max_x, added } };
	}

	if (cropDiff.size() == 0) return false;

	double minW = 999999.0;
	double idx = -1;
	for (int i = 0; i < cropDiff.size(); ++i) {
		BoostLineString &cropAddLs = cropDiff[i];
		double curW = lineWeight(cropAddLs);
		if (!bg::intersects(cropAddLs, bgAssemblyRef) && lineIsLegal(cropAddLs) && curW < minW)
		{
			minW = curW;
			idx = i;
		}
	}
	if (idx == -1) return false;
	for (int k = 0; k < cropDiff[idx].size() - 1; ++k) {
		drawLine(cropDiff[idx], k, sk);
	}

	return true;
}

double SilkScreenOutput::lineWeight(SilkSet &sk) {
	double assDist, cropDist;
	BoostLineString bgLineStr = sk.bgLineStr();
	skCropIsUnValue(bgLineStr, multBGCropperRef, cropGap, cropDist);
	skAssIsUnValue(bgLineStr, bgAssemblyRef, assemblygap, assDist);
	return cropDist + assDist;
}

double SilkScreenOutput::lineWeight(BoostLineString &ls) {
	double assDist, cropDist;
	skCropIsUnValue(ls, multBGCropperRef, cropGap, cropDist);
	skAssIsUnValue(ls, bgAssemblyRef, assemblygap, assDist);
	return cropDist + assDist;
}

void SilkScreenOutput::addCoordSafety(double added, bool isLower, bool isX) {
	SilkSet straightWay, aroundCropWay;
	
	for (int i = 0; i < legalSk.size(); ++i) {
		SilkSet &temp = legalSk[i];
		Silk head = temp.sk[0];
		Silk tail = temp.sk[temp.sk.size() - 1];
		if (isX) {
			if(canAddstraightLine(tail.x2, tail.y2, added, tail.y2, straightWay)) break;
			if (canAddstraightLine(added, head.y1, head.x1, head.y1, straightWay)) break;
		}
		else 
		{
			if (canAddstraightLine(tail.x2, tail.y2, tail.x2, added, straightWay)) break;
			if (canAddstraightLine(head.x1, added, head.x1, head.y1, straightWay)) break;
		}
	}
	
	for (int i = 0; i < legalSk.size(); ++i) {
		if (canAddAroundCrop(legalSk[i], added, isLower, isX, aroundCropWay)) break;
	}
	
	if (straightWay.sk.size() == 0 && aroundCropWay.sk.size() == 0) return;
	if (straightWay.sk.size() == 0) {
		legalSk.push_back(aroundCropWay);
		return;
	}
	if (aroundCropWay.sk.size() == 0) {
		legalSk.push_back(straightWay);
		return;
	}
	double straightW = lineWeight(straightWay);
	double aroundCropW = lineWeight(aroundCropWay);
	
	if (straightW < aroundCropW || almost_equal(straightW, aroundCropW)) 
	{
		legalSk.push_back(straightWay);
		return;
	}
	legalSk.push_back(aroundCropWay);

}

bool SilkScreenOutput::lineIsLegal(BoostLineString ls) {
	double cropDist, assDist;
	bool isCropValue = !skCropIsUnValue(ls, multBGCropperRef, cropGap, cropDist);
	bool isAssValue = !skAssIsUnValue(ls, bgAssemblyRef, assemblygap, assDist);
	//printf("assDist: %lf, cropDist: %lf\n", assDist, cropDist);
	//printf("isCropValue: %d, isAssValue: %d\n", isCropValue, isAssValue);
	return isCropValue && isAssValue;
}

void SilkScreenOutput::classifyLegal() {
	if (canWrite.size() == 0) {
		canWrite.resize(skSt.size());
		fill(canWrite.begin(), canWrite.end(), true);
	}
	double dist;
	for (int i = 0; i < canWrite.size(); ++i) {
		if (!canWrite[i]) continue;
		//legalSk.push_back(skSt[i]);
		if (lineIsLegal(skSt[i].bgLineStr()))
			legalSk.push_back(skSt[i]);
		else
			illegalSk.push_back(skSt[i]);
	}
}

bool SilkScreenOutput::isNeedModifty() {
	return illegalSk.size() > 0;
}

void SilkScreenOutput::curCloseIllegalSk(BoostLineString ls, vector<BoostLineString> &bgDiff, BoostLineString &curClose, double &dist) {
	double minClose = 9999.0;
	int idx = 0;
	for (int i = 0; i < bgDiff.size(); ++i) {
		double curDist = bg::distance(ls, bgDiff[i]);
		if (curDist < minClose)
			idx = i, minClose = curDist;
	}
	curClose = bgDiff[idx];
	dist = minClose;
}

void SilkScreenOutput::modiftyTheIllegalSk(Polygom &assembly, double addSafety, vector<BoostLineString> &bgDiff) {
	if (illegalSk.size() == 0) return;
	double curClose = 9999.0;
	for (int i = 0; i < illegalSk.size(); ++i) {
		//printf("illegal[%d]:\n", i);
		//printf("line: %d, arc: %d\n", illegalSk[i].line, illegalSk[i].arc);
		bool finded = false;
		double dist;
		BoostLineString rpSk;
		curCloseIllegalSk(illegalSk[i].bgLineStr(), bgDiff, rpSk, dist);
		
		vector<SilkSet> skStTemp;
		outputSilkscreen(skStTemp, rpSk, assembly.cyclePt, cyclePt, cycleList, assemblygap + addSafety, rAdoptRange);
		
		if (dist < 0.00005 || almost_equal(dist, 0.00005))
		{
			finded = true;
			//vector<SilkSet> skStTemp;
			//outputSilkscreen(skStTemp, rpSk, assembly.cyclePt, cyclePt, cycleList, assemblygap + addSafety);
			if (lineIsLegal(skStTemp[0].bgLineStr())) {
				legalSk.push_back(skStTemp[0]);
				illegalSk.erase(illegalSk.begin() + i);
				--i;
			}
		}
		else {
			illegalSk[i] = skStTemp[0];
		}
		/*if (finded) printf("finded\n\n");
		else printf("minClose:%lf,  not finded\n\n", dist);*/
	}
}

void SilkScreenOutput::finalLegalWay(BoostLineString outerLs) {
	SilkSet sk;
	legalSk.clear();
	for (int k = 0; k < outerLs.size() - 1; ++k) {
		drawLine(outerLs, k, sk);
	}
	legalSk.push_back(sk);
}

bool SilkScreenOutput::isLegalSkValue() {
	if (legalSk.size() == 0) {
		printf("Error: legalSk is 0!!!!\n");
		return false;
	}
	skStCoordSetUp(legalSk);
	bool cond1 = (
		!(skSt_min_x > as_min_x) &&
		!(skSt_max_x < as_max_x) &&
		!(skSt_min_y > as_min_y) &&
		!(skSt_max_y < as_max_y)
		);
	if (!cond1) printf("Error: Not Large Enough!!!!\n");;
 	return cond1;
}
