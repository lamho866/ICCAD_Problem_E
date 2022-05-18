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
	Cycle(double _x1, double _y1, double _x2, double _y2, double _rx, double _ry)
		:x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry) {
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
};

class Polygom {
public:
	vector<Cycle> cw;
	vector<Cycle> ccw;
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
			shape += boost::lexical_cast<std::string>(x1) + ' ' + boost::lexical_cast<std::string>(y1) + ',';
			Cycle c(atof(x1), atof(y1), atof(x2), atof(y2), atof(rx), atof(ry));

			double curX, curY, nxtX, nxtY;
			bool isCW = strcmp(tap, "CW") == 0;
			double maxDeg = c.deg(isCW);
			curX = c.x1, curY = c.y1;
			shape += boost::lexical_cast<std::string>(x1) + ' ' + boost::lexical_cast<std::string>(y1) + ',';
			for (double param = c.rDegSt; param < maxDeg - c.rDegSt; param += c.rDegSt) {
				c.rotationPt(curX, curY, nxtX, nxtY, isCW);
				shape += boost::lexical_cast<std::string>(nxtX) + ' ' + boost::lexical_cast<std::string>(nxtY) + ',';
				curX = nxtX, curY = nxtY;
			}
			shape += boost::lexical_cast<std::string>(x2) + ' ' + boost::lexical_cast<std::string>(y2) + ',';
		}

	}
	Polygom() {}
};

void makeThePolygonShape(Polygom &polyShape, BoostPolygon &bgPlogom) {
	polyShape.shape[polyShape.shape.size() - 1] = ' ';
	//printf("%s\n", polyShape.shape.c_str());
	bg::read_wkt("POLYGON((" + polyShape.shape + "))", bgPlogom);
}

void makeLine(Polygom &polyShape, BoostLineString &bgLineStr) {
	polyShape.shape[polyShape.shape.size() - 1] = ' ';
	//printf("%s\n", polyShape.shape.c_str());
	bg::read_wkt("LINESTRING(" + polyShape.shape + ")", bgLineStr);
}

void assemblyBuffer(Polygom &assembly, const double assemblygap, BoostMultiLineString &assemblyMultLine){
	BoostLineString assemblyLs;
	makeLine(assembly, assemblyLs);
	//make the outline
	BoostMultipolygon assemblyOutLs;
	bg::strategy::buffer::distance_symmetric<double> assemblygap_dist_strategy(assemblygap / 10.0);
	boost::geometry::buffer(assemblyLs, assemblyOutLs, assemblygap_dist_strategy, side_strategy, join_strategy, end_strategy, point_strategy);
	
	assert(assemblyOutLs.size() == 1);
	string strLs = boost::lexical_cast<std::string>(bg::wkt(assemblyOutLs.front()));
	string outLineStrLs = strLs.substr(9, strLs.find("),(") - 9);
	bg::read_wkt("LINESTRING(" + outLineStrLs + ")", assemblyLs);
	assemblyMultLine.push_back(assemblyLs);
}

void multiCropperBuffer(BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer) {
	BoostMultiLineString cropperMultLine;
	BoostLineString cropperLs;
	bg::strategy::buffer::distance_symmetric<double> cropper_dist_strategy(croppergap);
	boost::geometry::buffer(multiCropperLs, cropperMulLsBuffer, cropper_dist_strategy, side_strategy, join_strategy, end_strategy, point_strategy);
}

void buildAssemblyLine(Polygom &assembly, const double assemblygap, BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer, vector<BoostLineString> &bgDiff){
	BoostMultiLineString assemblyMultLine;
	assemblyBuffer(assembly, assemblygap, assemblyMultLine);
	multiCropperBuffer(multiCropperLs, croppergap, cropperMulLsBuffer);
	
	//difference
	bg::difference(assemblyMultLine, cropperMulLsBuffer, bgDiff);
}

bool isLargerEnough(BoostLineString silkScreen, const double silkscreenlen) {
	cout << "Len : " << bg::length(silkScreen) << endl;
	return (bg::length(silkScreen)) > silkscreenlen;
}

int main()
{
	Polygom assembly;
	BoostPolygon bgAssembly;

	vector<Polygom> cropperList;
	BoostMultipolygon multBGCropper;
	BoostMultipolygon cropperMulLsBuffer;//
	BoostMultiLineString multiCropperLs;
	BoostPolygon cropper;
	BoostLineString cropperLs;

	vector<BoostLineString> bgDiff;

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

	double x, y;
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
	
	buildAssemblyLine(assembly, assemblygap, multiCropperLs, croppergap, cropperMulLsBuffer, bgDiff);
	{
		std::ofstream svg("output.svg");
		boost::geometry::svg_mapper<BoostPoint> mapper(svg, 400, 400);
		mapper.add(bgAssembly);
		mapper.add(multBGCropper);
		mapper.add(cropperMulLsBuffer);

		for (int i = 0; i < bgDiff.size(); ++i) {
			if (bg::within(bgDiff[i], multBGCropper) == false && isLargerEnough(bgDiff[i], silkscreenlen)){
				mapper.add(bgDiff[i]);
				mapper.map(bgDiff[i], "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
			}
		}

		mapper.map(bgAssembly, "fill-opacity:0.5;fill:rgb(51,153,255);stroke:rgb(0,77,153);stroke-width:2");
		mapper.map(multBGCropper, "fill-opacity:0.5;fill:rgb(204,153,0);stroke:rgb(202,153,0);stroke-width:2");
		mapper.map(cropperMulLsBuffer, "fill-opacity:0.5;fill:rgb(255, 255, 153);stroke:rgb(77, 77, 0);stroke-width:2");
	}
}
