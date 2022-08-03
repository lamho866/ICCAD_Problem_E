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
	int cyclePtIdx;

	Silk();
	Silk(bool _isLine, double _x1, double _y1, double _x2, double _y2, double _rx = 0.0, double _ry = 0.0, bool _isCW = false, int _cyclePtIdx = -1);
	Silk(const Silk &sk);
	Silk& operator=(const Silk &sk);
	
	void write(ofstream &file);
};
