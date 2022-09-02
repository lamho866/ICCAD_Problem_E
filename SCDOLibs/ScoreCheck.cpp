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

	double sk_max_x, sk_max_y, sk_min_x, sk_min_y;
	double as_max_x, as_max_y, as_min_x, as_min_y;
	BoostLineString assemblyLs;
	makeLine(assembly, assemblyLs);
	findCoordMaxMin(assemblyLs, as_max_x, as_max_y, as_min_x, as_min_y);

	findCoordMaxMin(resultLs[0], sk_max_x, sk_max_y, sk_min_x, sk_min_y);
	for (int i = 1; i < resultLs.size(); ++i) {
		double cur_max_x, cur_max_y, cur_min_x, cur_min_y;
		findCoordMaxMin(resultLs[i], cur_max_x, cur_max_y, cur_min_x, cur_min_y);
		sk_max_x = max(sk_max_x, cur_max_x), sk_min_x = min(sk_min_x, cur_min_x);
		sk_max_y = max(sk_max_y, cur_max_y), sk_min_y = min(sk_min_y, cur_min_y);
	}

	double assemblyArea = score_assemblyAreaBuffer();
	double silkscreenArea = (sk_max_x - sk_min_x) * (sk_max_y - sk_min_y);
	printf("AssmblyArea : %lf, silkscreenArea : %lf\n", assemblyArea, silkscreenArea);
	double score = scoreCal((2.0 - silkscreenArea / assemblyArea), 0.25);
	printf("S1 = %lf\n", score);
	printf("");
	printf("assembly:\n");
	printf("maxX: %lf, maxY: %lf, minX: %lf, minY: %lf\n", as_max_x, as_max_y, as_min_x, as_min_y);
	printf("SilkSet:\n");
	printf("maxX: %lf, maxY: %lf, minX: %lf, minY: %lf\n", sk_max_x, sk_max_y, sk_min_x, sk_min_y);

	printf("");
	if (sk_max_x >= as_max_x &&
		sk_max_y >= as_max_y &&
		sk_min_x <= as_min_x &&
		sk_min_y <= as_min_y)
		printf("legal\n");
	else printf("illegal\n");
}
void ScoreCheck::scoreCase2(BoostLineString &assemblyLs) {
	//Part - A
	double silkScreenLen = 0.0;
	for (int i = 0; i < resultLs.size(); ++i)
		silkScreenLen += static_cast<double>(bg::length(resultLs[i]));
	double assemblyLen = static_cast<double>(bg::length(assemblyLs));
	double scoreA = 2 - (silkScreenLen / assemblyLen);

	scoreA = scoreCal(scoreA, 0.15);
	if (scoreA > 0.15) scoreA = 0.15;
	printf("-----------------------------------------\n");
	printf("Silkscreen similar score\n");
	printf("SilkscreenLen: %.5lf, assemblyOutLs: %5.lf\n", silkScreenLen, assemblyLen);
	printf("Part A (Perimeter): %.5lf\n\n", scoreA);

	//Part - B
	double lineDiff = abs(problemLA.line - resultLA.line), arcDiff = abs(problemLA.arc - resultLA.arc);
	printf("assLine : %d, assArc : %d\n", assembly.line, assembly.arc);
	printf("ResultLine : %d, ResultLineArc : %d\n", resultLA.line, resultLA.arc);
	printf("lineDiff : %lf, arcDiff : %lf\n", lineDiff, arcDiff);
	printf("Up : %lf, Down : %lf\n", (lineDiff + arcDiff), (static_cast<double>(multBGCropper.size()) + problemLA.line + problemLA.arc));
	double scoreB = 1 - ((lineDiff + arcDiff) / (static_cast<double>(multBGCropper.size()) + problemLA.line + problemLA.arc));
	scoreB = scoreCal(scoreB, 0.1);
	printf("Part B (LineArc Diff): %.5lf\n\n", scoreB);

	if (scoreA + scoreB <= 0.25)
		printf("S2 = %.5lf\n", scoreA + scoreB);
	else
		printf("S2 = 0.2500\n");
}

double ScoreCheck::avgCropperDistance(vector<BoostLineString> &resultLs, BoostMultipolygon &multBGCropper, vector<int> &illegalIdx, vector<double> &illDist, bool &isLegal) {
	double tCropper = 0.0, dist;
	for (int i = 0; i < resultLs.size(); ++i) {
		if (skCropIsUnValue(resultLs[i], multBGCropper, croppergap, dist) || true) {
			isLegal = false;
			illegalIdx.push_back(i);
			illDist.push_back(dist);
		}
		tCropper += dist;
	}
	tCropper /= static_cast<double>(resultLs.size());
	return tCropper;
}

void ScoreCheck::scoreCase3() {
	vector<int> illegalIdx;
	vector<double> illDist;
	bool isLegal = true;

	double tCropper = avgCropperDistance(resultLs, multBGCropper, illegalIdx, illDist, isLegal);
	double score = 1 - (tCropper - croppergap) * 10 / croppergap;
	printf("-----------------------------------------\n");
	printf("Cropper far\n");
	printf("tCropper : %.5lf, CropperGap : %.5lf\n", tCropper, croppergap);
	printf("%.5lf\n", score);
	score = scoreCal(score, 0.25);
	printf("S3 = %.5lf\n", score);
	if (isLegal) printf("legal\n");
	else {
		printf("illegal\n");
		for (int i = 0; i < illegalIdx.size(); ++i)
			printf("silkScreen[%d] : %lf, take4: %.4lf\n", illegalIdx[i], illDist[i], illDist[i]);
	}
}

void ScoreCheck::scoreCase4() {
	vector<int> illegalIdx;
	vector<double> illDist;
	bool isLegal = true;

	double tOutline = 0.0, dist;
	for (int i = 0; i < resultLs.size(); ++i) {
		if (skAssIsUnValue(resultLs[i], bgAssembly, assemblygap, dist) || true) {
			isLegal = false;
			illegalIdx.push_back(i);
			illDist.push_back(dist);
		}
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

	if (isLegal) printf("legal\n");
	else {
		printf("illegal\n");
		for (int i = 0; i < illegalIdx.size(); ++i)
			printf("silkScreen[%d] : %lf, take4: %.4lf\n", illegalIdx[i], illDist[i], illDist[i]);
	}
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

void ScoreCheck::readLineData(string s, double &x1, double &y1, double &x2, double &y2) {
	if (s[0] == 'l') {
		//isLine
		sscanf(s.c_str() + 5, "%lf,%lf,%lf,%lf\n", &x1, &y1, &x2, &y2);
		return;
	}
	if (s[0] == 'a') {
		char unUsed[45];
		sscanf(s.c_str() + 4, "%lf,%lf,%lf,%lf,%s\n", &x1, &y1, &x2, &y2, unUsed);
	}
}

void ScoreCheck::lineConnectTest() {
	string str;
	fstream input(rFile);
	double ftX, ftY;
	double x1, y1, x2, y2, preX, preY;
	bool isLegal = true, ft = true;
	input >> str;
	input >> str;
	input >> str;
	input >> str;

	while (input >> str) {
		if (str[0] == 's') {
			ft = true;
			continue;
		}
		if (ft) {
			readLineData(str, ftX, ftY, preX, preY);
			ft = false;
			continue;
		}
		readLineData(str, x1, y1, x2, y2);
		if (!almost_equal(preX, x1) || !almost_equal(preY, y1)) {
			isLegal = false;
		}
		preX = x2, preY = y2;
	}
	input.close();
	printf("silkScreen Is Connect Test\n");
	if (isLegal) printf("legal\n");
	else printf("illegal\n");
	printf("-----------------------------------------\n");
}

void ScoreCheck::silkScreenLenTest() {
	printf("silkScreenLenTest\n");
	bool isLegal = true;
	for (int i = 0; i < resultLs.size(); ++i)
		if (bg::length(resultLs[i]) < silkscreenlen) {
			isLegal = false;
			break;
		}
	if (isLegal) printf("legal\n");
	else printf("illegal\n");
	printf("-----------------------------------------\n");
}

void ScoreCheck::showScoreResult() {
	readContestFile();
	//printf("readContestFile Done\n");
	readResultFile();
	//printf("readResultFile Done\n");

	BoostMultiLineString assemblyMultLine;
	assemblyBuffer(assembly, assemblygap, assemblyMultLine);

	lineConnectTest();
	silkScreenLenTest();
	scoreCase1();
	scoreCase2(assemblyMultLine[0]);
	scoreCase3();
	scoreCase4();
	printf("------------------------------------------\n");
}
