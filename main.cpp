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
public: //CW or CCW 
	double x1, y1, x2, y2, rx, ry;
	Cycle(double _x1, double _y1, double _x2, double _y2, double _rx, double _ry)
		:x1(_x1), y1(_y1), x2(_x2), y2(_y2), rx(_rx), ry(_ry){}

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
			shape += boost::lexical_cast<std::string>(x1) + ' ' + boost::lexical_cast<std::string>(y1) + ',' + boost::lexical_cast<std::string>(x2) + ' ' + boost::lexical_cast<std::string>(y2) + ',';
			if (strcmp(tap, "CW") == 0)
				cw.push_back(Cycle(atof(x1), atof(y1), atof(x2), atof(y2), atof(rx), atof(ry)));
			else 
				ccw.push_back(Cycle(atof(x1), atof(y1), atof(x2), atof(y2), atof(rx), atof(ry)));
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

	//different part
	for (int i = 0; i < polyShape.ccw.size(); ++i) {
		makeCycle(polyShape.ccw[i], diffCycle);
		bg::difference(bgPlogom, diffCycle.front(), result);
		if(result.size() > 0)
			bgPlogom = result[0];
	}
	
	for (int i = 0; i < polyShape.cw.size(); ++i) {
		makeCycle(polyShape.ccw[i], unionCycle);
		bg::union_(bgPlogom, diffCycle.front(), result);
		if (result.size() > 0)
			bgPlogom = result[0];
	}
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
