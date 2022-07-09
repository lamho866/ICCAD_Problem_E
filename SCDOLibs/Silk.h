#pragma once
#include "Cycle.h"
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

class Silk : public Cycle{
private:
	void inputPoint(ofstream &file, const double x, const double y);
	void dist();
public:
	bool isLine;
	double len;
	Silk(bool _isLine, double _x1, double _y1, double _x2, double _y2, double _rx = 0.0, double _ry = 0.0, bool _isCW = false);
	
	void write(ofstream &file);

};
