#pragma once

#include "Silk.h"
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

class SilkSet
{
private:

public:
	vector<Silk> sk;
	int line, arc;
	double len;
	SilkSet();
	SilkSet(const SilkSet &skSt);
	SilkSet& operator=(const SilkSet& skSt);
	
	void write(ofstream &file);
	void addLine(double x1, double y1, double x2, double y2);
	void addCircle(double x1, double y1, double x2, double y2, double rx, double ry, bool isCW, int cyclePtIdx);
	void insertCircle(int i, double x1, double y1, double x2, double y2, double rx, double ry, bool isCW, int cyclePtIdx);
};
