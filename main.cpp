#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <boost/assign.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/io/svg/write_svg.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/algorithms/envelope.hpp>

#include <boost/foreach.hpp>

using namespace std;
#define PI 3.14159265

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;
typedef bg::model::multi_linestring<BoostLineString> BoostMultiLineString;

//define the strategies
const int points_per_circle = 36;
boost::geometry::strategy::buffer::join_miter join_strategy;
boost::geometry::strategy::buffer::end_flat end_strategy;
boost::geometry::strategy::buffer::side_straight side_strategy;
boost::geometry::strategy::buffer::point_circle point_strategy;

class Cycle
{
private:
	double dist(double x, double y) {
		return sqrt((x - rx) * (x - rx) + (y - ry) * (y - ry));
	}
public: //CW or CCW 
	const double rDegSt = 15.0;
	double x1, y1, x2, y2, rx, ry, r;
	bool isCW;
	Cycle(double _x1, double _y1, double _x2, double _y2, double _rx, double _ry, bool _isCW)
		:x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry), isCW(_isCW) {
		r = dist(x1, y1);
	}
	double deg(bool isCW) {
		if (x1 == x2 && y1 == y2)
			return 0.0;

		if (x1 == x2 || y1 == y2)
			return 180.0;

		double va_x = x1 - rx, va_y = y1 - ry;
		double vb_x = x2 - rx, vb_y = y2 - ry;
		double params = va_x * vb_y - va_y * vb_x;

		params /= dist(x1, y1);
		params /= dist(x2, y2);

		double degParams = asin(params) * 180 / PI;
		if (isCW) {
			if (degParams < 0) return -degParams;
			return 180 + degParams;
		}
		//CWW
		if (degParams < 0) return  degParams + 360;
		return degParams;
	}

	void rotationPt(double curX, double curY, double &nxtX, double &nxtY, bool isCW) {
		curX -= rx;
		curY -= ry;
		//the rotation is around the origin point
		double rDeg = (isCW) ? -rDegSt : rDegSt;
		nxtX = curX * cos(rDeg*PI / 180) - curY * sin(rDeg*PI / 180);
		nxtY = curX * sin(rDeg*PI / 180) + curY * cos(rDeg*PI / 180);

		nxtX += rx;
		nxtY += ry;
	}

	void drawArcycle(const double stX, const double stY, const double endX, const double endY, const double maxDeg, bool isCW, string &shape) {
		double curX = stX, curY = stY;
		double nxtX, nxtY;
		shape += boost::lexical_cast<std::string>(stX) + ' ' + boost::lexical_cast<std::string>(stY) + ',';
		for (double param = rDegSt; param < maxDeg - rDegSt; param += rDegSt) {
			rotationPt(curX, curY, nxtX, nxtY, isCW);
			shape += boost::lexical_cast<std::string>(nxtX) + ' ' + boost::lexical_cast<std::string>(nxtY) + ',';
			curX = nxtX, curY = nxtY;
		}
		shape += boost::lexical_cast<std::string>(endX) + ' ' + boost::lexical_cast<std::string>(endY) + ',';
	}
};

class Polygom {
public:
	vector<Cycle> cyclePt;
	string shape;
	void addLine(string &s) {
		char x1[10], x2[10], rx[10], y1[10], y2[10], ry[10], tap[10];
		for (int i = 0; i < s.size(); ++i)
			if (s[i] == ',')
				s[i] = ' ';
		if (s[0] == 'l') {
			//isLine
			sscanf(s.c_str() + 5, "%s %s %s %s\n", x1, y1, x2, y2);
			//printf("%s %s %s %s\n", x1, y1, x2, y2);
			shape += boost::lexical_cast<std::string>(x1) + ' ' + boost::lexical_cast<std::string>(y1) + ',' + boost::lexical_cast<std::string>(x2) + ' ' + boost::lexical_cast<std::string>(y2) + ',';
		}
		if (s[0] == 'a') {
			for (int i = 0; i < s.size(); ++i)
				if (s[i] == ',')
					s[i] = ' ';
			sscanf(s.c_str() + 4, "%s %s %s %s %s %s %s", x1, y1, x2, y2, rx, ry, tap);
			//printf("arc: %s %s %s %s %s %s %s\n", x1, y1, x2, y2, rx, ry);
			double curX, curY, nxtX, nxtY;
			bool isCW = strcmp(tap, "CW") == 0;

			Cycle c(atof(x1), atof(y1), atof(x2), atof(y2), atof(rx), atof(ry), isCW);
			cyclePt.push_back(c);
			double maxDeg = c.deg(isCW);

			c.drawArcycle(c.x1, c.y1, c.x2, c.y2, maxDeg, isCW, shape);
		}

	}
	Polygom() {}
};

void makeThePolygonShape(Polygom &polyShape, BoostPolygon &bgPlogom) {
	polyShape.shape[polyShape.shape.size() - 1] = ' ';
	//printf("%s\n", polyShape.shape.c_str());
	bg::read_wkt("POLYGON((" + polyShape.shape + "))", bgPlogom);
}

void makeThePolygonShape(string &shape, BoostPolygon &bgPlogom) {
	shape[shape.size() - 1] = ' ';
	//printf("%s\n", polyShape.shape.c_str());
	bg::read_wkt("POLYGON((" + shape + "))", bgPlogom);
}

void makeLine(Polygom &polyShape, BoostLineString &bgLineStr) {
	polyShape.shape[polyShape.shape.size() - 1] = ' ';
	//printf("%s\n", polyShape.shape.c_str());
	bg::read_wkt("LINESTRING(" + polyShape.shape + ")", bgLineStr);
}

void removeConnectPoint(BoostLineString &ls, const double tagX,const double tagY) {
	for (int i = 0; i < ls.size(); ++i) {
		if (bg::get<0>(ls[i]) == tagX && bg::get<1>(ls[i]) == tagY) {
			ls.erase(ls.begin() + i);
			return;
		}
	}
}

void assemblyBuffer(Polygom &assembly, const double assemblygap, BoostMultiLineString &assemblyMultLine) {
	BoostLineString assemblyLs;
	makeLine(assembly, assemblyLs);
	double tagX = bg::get<0>(assemblyLs[0]);
	double tagY = bg::get<1>(assemblyLs[0]);

	//make the outline
	BoostMultipolygon assemblyOutLs;
	bg::strategy::buffer::distance_symmetric<double> assemblygap_dist_strategy(assemblygap / 10.0);
	boost::geometry::buffer(assemblyLs, assemblyOutLs, assemblygap_dist_strategy, side_strategy, join_strategy, end_strategy, point_strategy);

	assert(assemblyOutLs.size() == 1);
	string strLs = boost::lexical_cast<std::string>(bg::wkt(assemblyOutLs.front()));
	string outLineStrLs = strLs.substr(9, strLs.find("),(") - 9);
	bg::read_wkt("LINESTRING(" + outLineStrLs + ")", assemblyLs);

	//remove the first point in the Line
	removeConnectPoint(assemblyLs, tagX, tagY);
	
	assemblyMultLine.push_back(assemblyLs);
}

void multiCropperBuffer(BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer) {
	BoostMultiLineString cropperMultLine;
	BoostLineString cropperLs;
	bg::strategy::buffer::distance_symmetric<double> cropper_dist_strategy(croppergap);
	boost::geometry::buffer(multiCropperLs, cropperMulLsBuffer, cropper_dist_strategy, side_strategy, join_strategy, end_strategy, point_strategy);
}

void buildAssemblyLine(Polygom &assembly, const double assemblygap, BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer, vector<BoostLineString> &bgDiff) {
	BoostMultiLineString assemblyMultLine;
	assemblyBuffer(assembly, assemblygap, assemblyMultLine);
	multiCropperBuffer(multiCropperLs, croppergap, cropperMulLsBuffer);

	//difference
	bg::difference(assemblyMultLine, cropperMulLsBuffer, bgDiff);
}

void makeCycleEachPoint(vector<Cycle> &cyclePt, const double assemblyGap, vector<BoostPolygon> &cycleList) {

	double addR = assemblyGap * 0.15;
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
	cout << "Len : " << bg::length(silkScreen) << endl;
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
	inputPoint(file, cycle.rx, cycle.ry);
	if (cycle.isCW)
		file << ",CW";
	else
		file << ",CCW";
	file << std::endl;
}

void drawLine(ofstream &file, BoostLineString &ls, int &i) {
	file << "line";
	inputPoint(file, ls[i]);
	inputPoint(file, ls[i + 1]);
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
		if (!inCycle) drawLine(file, ls, i);
	}
}

void checkoutPutResult(BoostPolygon &bgAssembly, BoostMultipolygon &multBGCropper, BoostMultipolygon &cropperMulLsBuffer) {
	string str;
	std::ifstream input("Result.txt");
	vector<BoostLineString> v_ls;
	BoostLineString ls;
	Polygom outline;

	//assembly
	input >> str;
	while (input >> str) {
		if (str[0] == 's') {
			makeLine(outline, ls);
			v_ls.push_back(ls);
			outline.shape = "";
			continue;
		}
		outline.addLine(str);
	}
	makeLine(outline, ls);
	v_ls.push_back(ls);

	{
		std::ofstream svg("checkResult.svg");
		boost::geometry::svg_mapper<BoostPoint> mapper(svg, 400, 400);

		mapper.add(bgAssembly);
		mapper.add(multBGCropper);
		mapper.add(cropperMulLsBuffer);

		for (int i = 0; i < v_ls.size(); ++i) {
			mapper.add(v_ls[i]);
			mapper.map(v_ls[i], "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
		}

		mapper.map(bgAssembly, "fill-opacity:0.5;fill:rgb(51,153,255);stroke:rgb(0,77,153);stroke-width:2");
		mapper.map(multBGCropper, "fill-opacity:0.5;fill:rgb(204,153,0);stroke:rgb(202,153,0);stroke-width:2");
		mapper.map(cropperMulLsBuffer, "fill-opacity:0.5;fill:rgb(255, 255, 153);stroke:rgb(77, 77, 0);stroke-width:2");

	}
}

int main()
{
	Polygom assembly;
	BoostPolygon bgAssembly;

	vector<Polygom> cropperList;
	BoostMultipolygon multBGCropper;
	BoostMultiLineString multiCropperLs;
	BoostPolygon cropper;
	BoostLineString cropperLs;

	string str;
	double assemblygap, croppergap, silkscreenlen;
	int cropSize = 0;

	std::ifstream input("Problem.txt");
	input >> str;
	assemblygap = atof(str.substr(12).c_str());
	input >> str;
	croppergap = atof(str.substr(10).c_str());
	input >> str;
	silkscreenlen = atof(str.substr(14).c_str());
	printf("silkscreenlen %lf\n", silkscreenlen);
	input >> str;

	//assembly
	while (1) {
		input >> str;
		if (str[0] == 'c') break;
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

	vector<BoostLineString> bgDiff;
	BoostMultipolygon cropperMulLsBuffer;
	buildAssemblyLine(assembly, assemblygap, multiCropperLs, croppergap, cropperMulLsBuffer, bgDiff);

	vector<BoostPolygon> cycleList;
	makeCycleEachPoint(assembly.cyclePt, assemblygap, cycleList);
	printf("CycleList size : %d\n", cycleList.size());

	ofstream resultFile("Result.txt");
	{
		std::ofstream svg("output.svg");
		boost::geometry::svg_mapper<BoostPoint> mapper(svg, 400, 400);
		mapper.add(bgAssembly);
		mapper.add(multBGCropper);
		mapper.add(cropperMulLsBuffer);

		for (int i = 0; i < cycleList.size(); ++i) {
			mapper.add(cycleList[i]);
			mapper.map(cycleList[i], "fill-opacity:0.5;fill:rgb(255, 102, 255);stroke:rgb(102, 0, 102);stroke-width:2");
		}

		for (int i = 0; i < bgDiff.size(); ++i) {
			if (bg::within(bgDiff[i], multBGCropper) == false && isLargerEnough(bgDiff[i], silkscreenlen)) {
				outputSilkscreen(resultFile, bgDiff[i], cycleList, assembly.cyclePt);
				mapper.add(bgDiff[i]);
				mapper.map(bgDiff[i], "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
			}
		}

		mapper.map(bgAssembly, "fill-opacity:0.5;fill:rgb(51,153,255);stroke:rgb(0,77,153);stroke-width:2");
		mapper.map(multBGCropper, "fill-opacity:0.5;fill:rgb(204,153,0);stroke:rgb(202,153,0);stroke-width:2");
		mapper.map(cropperMulLsBuffer, "fill-opacity:0.5;fill:rgb(255, 255, 153);stroke:rgb(77, 77, 0);stroke-width:2");
	}
	resultFile.close();

	checkoutPutResult(bgAssembly, multBGCropper, cropperMulLsBuffer);
	system("pause");
}