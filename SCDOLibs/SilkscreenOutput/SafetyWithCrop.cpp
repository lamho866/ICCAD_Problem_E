#include "SafetyWithCrop.h"

boost::geometry::strategy::buffer::join_miter joinStr;
boost::geometry::strategy::buffer::end_flat endStr;
boost::geometry::strategy::buffer::side_straight sideStr;
boost::geometry::strategy::buffer::point_circle pointStr;

void makeSafetyWithCrop(BoostPolygon crop, double tagX, double tagY, BoostMultipolygon &assBuffer, vector<BoostLineString> &cropDiff) {
	string strLs = boost::lexical_cast<std::string>(bg::wkt(crop.outer()));
	printf("%s \n\n", strLs.c_str());
	string outStr = strLs.substr(9, strLs.size() - 11);
	printf("%s \n", outStr.c_str());
	BoostLineString outerLs;
	BoostMultiLineString mulOuterLs;
	bg::read_wkt("LINESTRING(" + outStr + ")", outerLs);

	//mulOuterLs.push_back(outerLs);
	//bg::difference(mulOuterLs, assBuffer, cropDiff);
	
	cropDiff.push_back(outerLs);
	printf("TagX: %lf\n", tagX);
	cutCrop(tagX, true, cropDiff);
	cutCrop(tagY, false, cropDiff);
}

bool cutState(double x, double y, double tag, double isX) {
	if (isX) return x < tag;
	return y < tag;
}

void cutLineByAxis(BoostLineString ls, double tag, bool isX, vector<BoostLineString> &temp) {
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

			isLower = cutState(x, y, tag, isX);
		}
		tempLs.push_back(BoostPoint(x, y));
	}
	temp.push_back(tempLs);
}

void cutCrop(double tag, bool isX, vector<BoostLineString> &cropDiff) {
	vector<BoostLineString> temp;
	for (int i = 0; i < cropDiff.size(); ++i)
		cutLineByAxis(cropDiff[i], tag, isX, temp);
	printf("TempLs:\n");
	for (int i = 0; i < temp.size(); ++i)
		printf("%s\n", boost::lexical_cast<std::string>(bg::wkt(temp[i])).c_str());
	printf("\n");
	cropDiff = temp;
}
