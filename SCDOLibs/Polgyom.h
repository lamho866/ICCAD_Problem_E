#pragma once
#include "Cycle.h"
#include <vector>
#include <string>
using namespace std;

class Polygom {
public:
	vector<Cycle> cyclePt;
	string shape;
	void addLine(string &s);
	Polygom() {}
};