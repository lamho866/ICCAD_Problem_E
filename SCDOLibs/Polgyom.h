#pragma once
#include "Cycle.h"
#include <vector>
#include <string>
using namespace std;

class Polygom {
private:
	bool isNear(double x1, double y1, double x2, double y2);
	bool isConnect(Cycle a, Cycle b);
public:
	vector<Cycle> cyclePt;
	string shape;
	void addLine(string &s);
	void cyclePtCombe();
	Polygom() {}
};