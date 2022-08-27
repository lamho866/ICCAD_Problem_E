#pragma once

#include "Silk.h"
#include "Polygom.h"
#include "Common.h"
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
	
	double max_x, max_y, min_x, min_y;
	int max_x_cnt, max_y_cnt, min_x_cnt, min_y_cnt;

	double len;
	SilkSet();
	SilkSet(const SilkSet &skSt);
	SilkSet& operator=(const SilkSet &skSt);
	bool operator< (SilkSet const& b) const;
	
	void updateMinMaxCoord();
	void write(ofstream &file);
	void addLine(double x1, double y1, double x2, double y2);
	void addCircle(double x1, double y1, double x2, double y2, double rx, double ry, bool isCW, int cyclePtIdx);
	void insertCircle(int i, double x1, double y1, double x2, double y2, double rx, double ry, bool isCW, int cyclePtIdx);
	void insertLine(int i, double x1, double y1, double x2, double y2);
	BoostLineString bgLineStr();
};

bool minXCmp(const SilkSet &a,const SilkSet &b);
bool minYCmp(const SilkSet &a,const SilkSet &b);
bool maxXCmp(const SilkSet &a,const SilkSet &b);
bool maxYCmp(const SilkSet &a,const SilkSet &b);
