#include "Silk.h"

Silk::Silk(bool _isLine, double _x1, double _y1, double _x2, double _y2, double _rx, double _ry, bool _isCW, int _cyclePtIdx) : isLine(_isLine), cyclePtIdx(_cyclePtIdx), Cycle(_x1, _y1, _x2, _y2, _rx, _ry, _isCW){
	dist();
}

Silk::Silk(): isLine(false), len(0.0), cyclePtIdx(-1), Cycle(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false) {}

Silk::Silk(const Silk &sk) : isLine(sk.isLine), cyclePtIdx(sk.cyclePtIdx), Cycle(sk.x1, sk.y1, sk.x2, sk.y2, sk.rx, sk.ry, sk.isCW){}

Silk& Silk::operator=(const Silk& sk) {
	x1 = sk.x1, x2 = sk.x2;
	y1 = sk.y1, y2 = sk.y2;
	rx = sk.rx, ry = sk.ry;
	r = sk.r;

	stDeg = sk.stDeg, edDeg = sk.edDeg;
	isCW = sk.isCW;
	
	isLine = sk.isLine;
	len = sk.len;
	cyclePtIdx = sk.cyclePtIdx;

	return *this;
}

void Silk::dist() {
	if (isLine) {
		len = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
		return;
	}
	len = 2 * PI * r * deg(isCW) / 360.0;
}

void Silk::inputPoint(ofstream &file, const double x, const double y){
    file << ',' << fixed << setprecision(4) << x << ',' << fixed << setprecision(4) << y;
}

void Silk::write(ofstream &file){
    if(isLine){
        file << "line";
        inputPoint(file, x1, y1);
        inputPoint(file, x2, y2);
        file << endl;
        return;
    }

    file << "arc";
    inputPoint(file, x1, y1);
	inputPoint(file, x2, y2);
    inputPoint(file, rx, ry);
	if (isCW)
		file << ",CW";
	else
		file << ",CCW";
    file << endl;
}
