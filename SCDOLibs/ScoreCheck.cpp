#include "ScoreCheck.h"
//LineArc
void LineArc::readLine(string s) {
	if (s[0] == 'l') {	
		line++;
	}
	if (s[0] == 'a') {
		arc++;
	}
}

//ScoreCheck
ScoreCheck::ScoreCheck(string &_pFile, string &_rFile) :pFile(_pFile), rFile(_rFile) {
	pFile += ".txt";
	rFile += ".txt";
}

double ScoreCheck::scoreCal(double score, double percent) {
	if (score <= 0.0) return 0.0;
	return score *= percent;
}

void ScoreCheck::scoreCase1() {}
void ScoreCheck::scoreCase2(BoostLineString &assemblyLs) {
	//Part - A
	double silkScreenLen = 0.0;
	for (int i = 0; i < resultLs.size(); ++i)
		silkScreenLen += static_cast<double>(bg::length(resultLs[i]));
	double assemblyLen = static_cast<double>(bg::length(assemblyLs));
	double scoreA = 2 - (silkScreenLen / assemblyLen);

	scoreA = scoreCal(scoreA, 0.15);
	printf("SilkscreenLen: %.5lf, assemblyOutLs: %5.lf\n", silkScreenLen, assemblyLen);
	printf("Part A : %.5lf\n", scoreA);

	//Part - B
	double lineDiff = abs(problemLA.line - resultLA.line), arcDiff = abs(problemLA.arc - resultLA.arc);
	double scoreB = 1 - ((lineDiff + arcDiff) / (static_cast<double>(multBGCropper.size()) + problemLA.line + problemLA.arc));
	scoreB = scoreCal(scoreB, 0.1);
	printf("Part B : %.5lf\n", scoreB);
	printf("Part-2 score: %.5lf\n", scoreA + scoreB);
}

double ScoreCheck::avgCropperDistance(vector<BoostLineString> &resultLs, BoostMultipolygon &multBGCropper) {
	double tCropper = 0.0;
	for (int i = 0; i < resultLs.size(); ++i) {
		double curMin = 999999.0;
		for (int j = 0; j < multBGCropper.size(); ++j)
		{
			double dist = static_cast<double>(bg::distance(multBGCropper[j], resultLs[i]));
			curMin = min(curMin, dist);
		}
		tCropper += curMin;
	}
	tCropper /= static_cast<double>(resultLs.size());
}

void ScoreCheck::scoreCase3() {
	double tCropper = avgCropperDistance(resultLs, multBGCropper);
	double score = 1 - (tCropper - croppergap) * 10 / croppergap;
	printf("%.5lf %.5lf\n", tCropper, croppergap);
	printf("%.5lf\n", score);
	score = scoreCal(score, 0.25);
	printf("Part-3 %.5lf\n", score);
}

void ScoreCheck::scoreCase4() {
	double tOutline = 0.0;
	for (int i = 0; i < resultLs.size(); ++i) {
		double dist = bg::distance(bgAssembly, resultLs[i]);
		tOutline += dist;
	}
	tOutline /= static_cast<double>(resultLs.size());
	double score = 1 - (tOutline - assemblygap) * 10 / assemblygap;
	printf("%.5lf %.5lf\n", tOutline, assemblygap);
	printf("%.5lf\n", score);
	score = scoreCal(score, 0.25);
	printf("Part-4 %.5lf\n", score);
}

void ScoreCheck::readContestFile() {
	BoostPolygon cropper;
	vector<Polygom> cropperList;
	BoostMultiLineString multiCropperLs;
	BoostLineString cropperLs;

	int cropSize = 0;
	string str;

	std::ifstream input(pFile);
	input >> str;
	assemblygap = atof(str.substr(12).c_str());
	input >> str;
	croppergap = atof(str.substr(10).c_str());
	input >> str;
	silkscreenlen = atof(str.substr(14).c_str());
	input >> str;

	//assembly
	while (1) {
		input >> str;
		if (str[0] == 'c') break;
		problemLA.readLine(str);
		assembly.addLine(str);
	}
	makeThePolygonShape(assembly, bgAssembly);

	//cropper
	cropperList.push_back(Polygom());
	while (input >> str) {
		if (str[0] == 'c') {
			makeThePolygonShape(cropperList[cropSize], cropper);
			makeLine(cropperList[cropSize], cropperLs);
			multBGCropper.push_back(cropper);
			multiCropperLs.push_back(cropperLs);
			cropper.clear();
			cropperList.push_back(Polygom());
			cropSize++;
			continue;
		}
		cropperList[cropSize].addLine(str);
	}
	makeThePolygonShape(cropperList[cropSize], cropper);
	makeLine(cropperList[cropSize], cropperLs);
	multBGCropper.push_back(cropper);
	multiCropperLs.push_back(cropperLs);
	cropSize++;
	input.close();
}

void ScoreCheck::readResultFile() {
	string str;
	fstream input(rFile);
	BoostLineString ls;
	Polygom outline;

	input >> str;
	while (input >> str) {
		if (str[0] == 's') {
			resultLA.readLine(str);
			makeLine(outline, ls);
			resultLs.push_back(ls);
			outline.shape = "";
			continue;
		}
		outline.addLine(str);
	}
	resultLA.readLine(str);
	makeLine(outline, ls);
	resultLs.push_back(ls);
	input.close();
}

void ScoreCheck::showScoreResult() {
	readContestFile();
	printf("readContestFile Done\n");
	readResultFile();
	printf("readResultFile Done\n");

	BoostMultiLineString assemblyMultLine;
	assemblyBuffer(assembly, assemblygap, assemblyMultLine);

	scoreCase2(assemblyMultLine[0]);
	scoreCase3();
	scoreCase4();
}
