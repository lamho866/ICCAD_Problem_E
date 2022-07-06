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

double ScoreCheck::score_assemblyAreaBuffer() {

	const int points_per_circle = 36;
	boost::geometry::strategy::buffer::join_miter join_strategy;
	boost::geometry::strategy::buffer::end_flat end_strategy;
	boost::geometry::strategy::buffer::side_straight side_strategy;
	boost::geometry::strategy::buffer::point_circle point_strategy;

	BoostMultipolygon bufferedAssebly;
	bg::strategy::buffer::distance_symmetric<double> assemblygap_dist_strategy(assemblygap);
	boost::geometry::buffer(bgAssembly, bufferedAssebly, assemblygap_dist_strategy, side_strategy, join_strategy, end_strategy, point_strategy);

	return static_cast<double>(bg::area(bufferedAssebly));
}

void ScoreCheck::scoreCase1() {
	double max_x = 0.0, max_y = 0.0;
	double min_x = 9999.0, min_y = 9999.0;

	for (int i = 0; i < resultLs.size(); ++i) {
		BoostLineString curLs = resultLs[i];
		for (int j = 0; j < curLs.size(); ++j) {
			double x, y;
			getPointData(curLs[j], x, y);
			max_x = max(max_x, x), max_y = max(max_y, y);
			min_x = min(min_x, x), min_y = min(min_y, y);
		}
	}

	double assemblyArea = score_assemblyAreaBuffer();
	double silkscreenArea = (max_x - min_x) * (max_y - min_y);
	printf("AssmblyArea : %lf, silkscreenArea : %lf\n", assemblyArea, silkscreenArea);
	double score = scoreCal( (2.0 - silkscreenArea / assemblyArea) , 0.25);
	printf("S1 = %lf\n", score);
}
void ScoreCheck::scoreCase2(BoostLineString &assemblyLs) {
	//Part - A
	double silkScreenLen = 0.0;
	for (int i = 0; i < resultLs.size(); ++i)
		silkScreenLen += static_cast<double>(bg::length(resultLs[i]));
	double assemblyLen = static_cast<double>(bg::length(assemblyLs));
	double scoreA = 2 - (silkScreenLen / assemblyLen);

	scoreA = scoreCal(scoreA, 0.15);
	printf("-----------------------------------------\n");
	printf("Silkscreen similar score\n");
	printf("SilkscreenLen: %.5lf, assemblyOutLs: %5.lf\n", silkScreenLen, assemblyLen);
	printf("Part A (Perimeter): %.5lf\n", scoreA);

	//Part - B
	double lineDiff = abs(problemLA.line - resultLA.line), arcDiff = abs(problemLA.arc - resultLA.arc);
	printf("lineDiff : %lf, arcDiff : %lf\n", lineDiff, arcDiff);
	printf("Up : %lf, Down : %lf\n", (lineDiff + arcDiff), (static_cast<double>(multBGCropper.size()) + problemLA.line + problemLA.arc));
	double scoreB = 1 - ((lineDiff + arcDiff) / (static_cast<double>(multBGCropper.size()) + problemLA.line + problemLA.arc));
	scoreB = scoreCal(scoreB, 0.1);
	printf("Part B (LineArc Diff): %.5lf\n", scoreB);

	if (scoreA + scoreB <= 0.25)
		printf("S2 = %.5lf\n", scoreA + scoreB);
	else
		printf("S2 = 0.2500\n");
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
	return tCropper;
}

void ScoreCheck::scoreCase3() {
	double tCropper = avgCropperDistance(resultLs, multBGCropper);
	double score = 1 - (tCropper - croppergap) * 10 / croppergap;
	printf("-----------------------------------------\n");
	printf("Cropper far\n");
	printf("tCropper : %.5lf, CropperGap : %.5lf\n", tCropper, croppergap);
	printf("%.5lf\n", score);
	score = scoreCal(score, 0.25);
	printf("S3 = %.5lf\n", score);
}

void ScoreCheck::scoreCase4() {
	double tOutline = 0.0;
	for (int i = 0; i < resultLs.size(); ++i) {
		double dist = bg::distance(bgAssembly, resultLs[i]);
		tOutline += dist;
	}
	tOutline /= static_cast<double>(resultLs.size());
	double score = 1 - (tOutline - assemblygap) * 10 / assemblygap;
	printf("-----------------------------------------\n");
	printf("Assembly far\n");
	printf("tOutLine : %.5lf, assemblyGap %.5lf\n", tOutline, assemblygap);
	printf("%.5lf\n", score);
	score = scoreCal(score, 0.25);
	printf("S4 = %.5lf\n", score);
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
			makeLine(outline, ls);
			resultLs.push_back(ls);
			outline.shape = "";
			continue;
		}
		resultLA.readLine(str);
		outline.addLine(str);
	}
	resultLA.readLine(str);
	makeLine(outline, ls);
	resultLs.push_back(ls);
	input.close();
}

void ScoreCheck::showScoreResult() {
	readContestFile();
	//printf("readContestFile Done\n");
	readResultFile();
	//printf("readResultFile Done\n");

	BoostMultiLineString assemblyMultLine;
	assemblyBuffer(assembly, assemblygap, assemblyMultLine);

	scoreCase1();
	scoreCase2(assemblyMultLine[0]);
	scoreCase3();
	scoreCase4();
	printf("---------------------------\n");
}
