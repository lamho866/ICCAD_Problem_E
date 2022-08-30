#include "SafetyWithCrop.h"

boost::geometry::strategy::buffer::join_miter joinStr;
boost::geometry::strategy::buffer::end_flat endStr;
boost::geometry::strategy::buffer::side_straight sideStr;
boost::geometry::strategy::buffer::point_circle pointStr;

void makeSafetyWithCrop(BoostPolygon crop, double tagX, double tagY, vector<BoostLineString> &cropDiff) {
	string strLs = boost::lexical_cast<std::string>(bg::wkt(crop.outer()));
	string outStr = strLs.substr(9, strLs.size() - 11);
	BoostLineString outerLs;
	bg::read_wkt("LINESTRING(" + outStr + ")", outerLs);
	cropDiff.push_back(outerLs);

	cutCrop(tagY, false, false, cropDiff);
	printf("cutSize ft: %d\n", cropDiff.size());
	cutCrop(tagX, true, true, cropDiff);
	printf("cutSize sd: %d\n", cropDiff.size());
}

bool cutState(double x, double y, double tag, double isX) {
	if (isX) return x < tag;
	return y < tag;
}

void connectLine(BoostLineString &ls, BoostLineString &addLs) {
	for (int i = 0; i < addLs.size(); ++i)
		ls.push_back(addLs[i]);
}

void cutLineByAxis(BoostLineString ls, double tag, bool isX, bool isSecond, vector<BoostLineString> &temp) {
	BoostLineString tempLs;
	double x, y;
	getPointData(ls[0], x, y);
	bool isLower = cutState(x, y, tag, isX);
	for (int i = 0; i < ls.size(); ++i) {
		getPointData(ls[i], x, y);
		if (cutState(x, y, tag, isX) != isLower)
		{
			if (isX) tempLs.push_back(BoostPoint(tag, y));
			else tempLs.push_back(BoostPoint(x, tag));
			
			temp.push_back(tempLs);
			tempLs.clear();
			
			if (isX) tempLs.push_back(BoostPoint(tag, y));
			else tempLs.push_back(BoostPoint(x, tag));

			isLower = !isLower;
		}
		tempLs.push_back(BoostPoint(x, y));
	}
	
	if (isSecond || temp.size() == 0) {
		temp.push_back(tempLs);
		return;
	}
	double stX, stY, edX, edY;
	getPointData(temp[0][0], stX, stY);
	getPointData(tempLs[tempLs.size() - 1], edX, edY);

	if (cutState(stX, stY, tag, isX) == cutState(edX, edY, tag, isX) &&
		almost_equal(stX, edX) && almost_equal(stY, edY)) 
	{
		connectLine(tempLs, temp[0]);
		temp[0] = tempLs;
	}
	
}

void cutCrop(double tag, bool isX, bool isSecond,vector<BoostLineString> &cropDiff) {
	if (cropDiff.size() == 0) return;
	vector<BoostLineString> temp;
	for (int i = 0; i < cropDiff.size(); ++i)
		cutLineByAxis(cropDiff[i], tag, isX, isSecond, temp);
	cropDiff = temp;
}
