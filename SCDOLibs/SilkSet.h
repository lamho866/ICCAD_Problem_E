#pragma once

#include "Silk.h"
#include <vector>
#include <fstream>
using namespace std;

class SilkSet
{
private:

public:
	vector<Silk> sk;
	int line, arc, len;
	SilkSet();
	void write(ofstream &file);
	void addLine(double &x1, double &y1, double &x2, double &y2);
	void addCircle(double &x1, double &y1, double &x2, double &y2, double &rx, double &ry, bool &isCW);
};
