#include "Silk.h"

Silk::Silk(bool _isLine, double _x1, double _y1, double _x2, double _y2, double _rx, double _ry, bool _isCW) : isLine(_isLine), Cycle(_x1, _y1, _x2, _y2, _rx, _ry, _isCW){
	dist();
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
