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

using namespace std;
#define PI 3.14159265

namespace bg = boost::geometry;
typedef bg::model::d2::point_xy<double> BoostPoint;
typedef bg::model::linestring<BoostPoint> BoostLineString;
typedef bg::model::polygon<BoostPoint> BoostPolygon;
typedef bg::model::multi_polygon<BoostPolygon> BoostMultipolygon;

#define points_per_circle 36
boost::geometry::strategy::buffer::join_round join_strategy(points_per_circle);
boost::geometry::strategy::buffer::end_round end_strategy(points_per_circle);
boost::geometry::strategy::buffer::point_circle circle_strategy(points_per_circle);
boost::geometry::strategy::buffer::side_straight side_strategy;

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
		if (s[0] == 'l') {
			//isLine
			string subS = s.substr(5);
			int i = 0, j = subS.size() - 1;
			while (subS[i] != ',') ++i; subS[i] = ' ';
			while (subS[j] != ',') --j; subS[j] = ' ';
			shape += subS + ',';
		}
		if (s[0] == 'a') {
			for (int i = 0; i < s.size(); ++i)
				if (s[i] == ',')
					s[i] = ' ';
			char x1[10], x2[10], rx[10], y1[10], y2[10], ry[10], tap[10];
			sscanf(s.c_str() + 4, "%s %s %s %s %s %s %s", x1, y1, x2, y2, rx, ry, tap);
			///printf("arc: %s %s %s %s %s %s %s ", x1, y1, x2, y2, rx, ry);
			shape += boost::lexical_cast<std::string>(x1) + ' ' + boost::lexical_cast<std::string>(y1) + ',';
			Cycle c(atof(x1), atof(y1), atof(x2), atof(y2), atof(rx), atof(ry));

			double curX, curY, nxtX, nxtY;
			bool isCW = strcmp(tap, "CW") == 0;
			double maxDeg = c.deg(isCW);
			curX = c.x1, curY = c.y1;
			for (double param = c.rDegSt; param < maxDeg - c.rDegSt; param += c.rDegSt) {
				c.rotationPt(curX, curY, nxtX, nxtY, isCW);
				shape += boost::lexical_cast<std::string>(nxtX) + ' ' + boost::lexical_cast<std::string>(nxtY) + ',';
				curX = nxtX, curY = nxtY;
			}
		}

	}
	Polygom() {}
};

void makeCycle(Cycle &cycle, BoostMultipolygon &multiPolygonCycle) {
	BoostPoint pointA(cycle.x1, cycle.y1);
	BoostPoint centerCycle(cycle.rx, cycle.ry);

	double dist = bg::distance(pointA, centerCycle);
	boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(dist);
	boost::geometry::buffer(centerCycle, multiPolygonCycle, distance_strategy, side_strategy, join_strategy, end_strategy, circle_strategy);
}

void makeThePolygonShape(BoostPolygon &bgPlogom, Polygom &polyShape) {
	BoostMultipolygon unionCycle;
	BoostMultipolygon diffCycle;
	vector<BoostPolygon> result;

	polyShape.shape[polyShape.shape.size() - 1] = ' ';
	bg::read_wkt("POLYGON((" + polyShape.shape + "))", bgPlogom);
}


int main()
{
	Polygom assembly;
	BoostPolygon bgAssembly;

	vector<Polygom> cropperList;
	BoostMultipolygon multBGCropper;
	BoostPolygon cropper;

	string str;
	double assemblygap, croppergap, silkscreenlen;
	int cropSize = 0;

	std::ifstream input("Problem.txt");
	input >> str;
	assemblygap = atof(str.substr(12).c_str());
	//printf("assemblygap: %lf\n", assemblygap);
	input >> str;
	//sscanf(str, "%lf", &croppergap);
	input >> str;
	//sscanf(str, "%lf", &silkscreenlen);
	input >> str;

	double x, y;
	//assembly
	while (1) {
		input >> str;
		if (str[0] == 'c') break;
		assembly.addLine(str);
	}
	makeThePolygonShape(bgAssembly, assembly);

	//cropper
	cropperList.push_back(Polygom());
	while (input >> str) {
		if (str[0] == 'c') {
			makeThePolygonShape(cropper, cropperList[cropSize]);
			multBGCropper.push_back(cropper);
			cropperList.push_back(Polygom());
			cropSize++;
			continue;
		}
		cropperList[cropSize].addLine(str);
	}
	makeThePolygonShape(cropper, cropperList[cropSize]);
	multBGCropper.push_back(cropper);
	cropSize++;
	{
		std::ofstream svg("output.svg");
		boost::geometry::svg_mapper<BoostPoint> mapper(svg, 400, 400);
		mapper.add(bgAssembly);
		mapper.add(multBGCropper);

		mapper.map(bgAssembly, "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
		mapper.map(multBGCropper, "fill-opacity:0.5;fill:rgb(204,153,0);stroke:rgb(202,153,0);stroke-width:2");
	}
}
