#include "SkValueCheck.h"


bool skCropIsUnValue(BoostLineString &sk, BoostMultipolygon &multBGCropper, double cropGap, double &minDist) {
	double curMin = 999999.0;
	for (int j = 0; j < multBGCropper.size(); ++j)
	{
		double dist = static_cast<double>(bg::distance(multBGCropper[j], sk));
		curMin = min(curMin, dist);
	}
	minDist = curMin;
	return minDist < cropGap;
}

bool skAssIsUnValue(BoostLineString &sk, BoostPolygon &bgAssembly, double assGap, double &minDist) {
	minDist = bg::distance(bgAssembly, sk);
	return minDist < assGap;
}
