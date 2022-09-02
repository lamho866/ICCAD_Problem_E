#pragma once
#include "Cycle.h"
#include "Silk.h"
#include <vector>
#include <string>
using namespace std;

class Polygom {
private:
	bool isNear(double x1, double y1, double x2, double y2);
	bool isConnect(Cycle a, Cycle b);
public:
	vector<Cycle> cyclePt;
	vector<Silk> sk;
	string shape;
	int line, arc;
	void addLine(string &s);
	void cyclePtCombe();
	void inputLine(double &x1, double &y1, double &x2, double &y2);
	void inputArc(Cycle &c);
	Polygom();
};
