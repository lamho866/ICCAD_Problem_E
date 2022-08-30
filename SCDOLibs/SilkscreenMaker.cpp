#include "SilkscreenMaker.h"

const int points_per_circle = 36;
boost::geometry::strategy::buffer::join_miter join_strategy;
boost::geometry::strategy::buffer::end_flat end_strategy;
boost::geometry::strategy::buffer::side_straight side_strategy;
boost::geometry::strategy::buffer::point_circle point_strategy;

template <typename T>
int countStartPoint(T &ls, const double tagX, const double tagY) {
	int cnt = 0;
	for (int i = 0; i < ls.size(); ++i) {
		double x, y;
		getPointData(ls[i], x, y);
		if (almost_equal(x, tagX) && almost_equal(y, tagY))
			cnt++;
	}
	return cnt;
}

template <typename T>
void removeConnectPoint(T &ls, const double tagX, const double tagY) {
	for (int i = 0; i < ls.size(); ++i) {
		double x, y;
		getPointData(ls[i], x, y);
		if (almost_equal(x, tagX) && almost_equal(y, tagY)) {
			//BoostPoint newPt;
			ls.erase(ls.begin() + i);
			return;
		}
	}
}

struct SegmentLine
{
	double x1, y1, x2, y2;
	SegmentLine(double _x1, double _y1, double _x2, double _y2) : x1(_x1), x2(_x2), y1(_y1), y2(_y2) {}
};

void connectAB(SegmentLine &a, SegmentLine &b, double &res_x, double &res_y) {
	double a1, b1, c1, a2, b2, c2;
	double D, Dx, Dy;
	a1 = a.y1 - a.y2, b1 = -a.x1 + a.x2;
	a2 = b.y1 - b.y2, b2 = -b.x1 + b.x2;
	c1 = a1 * a.x1 + b1 * a.y1;
	c2 = a2 * b.x1 + b2 * b.y1;

	D = a1 * b2 - a2 * b1;
	Dx = c1 * b2 - c2 * b1;
	Dy = a1 * c2 - a2 * c1;

	res_x = Dx / D;
	res_y = Dy / D;
}

template <typename T>
void modifyStartPoint(T &ls, const double tagX, const double tagY) {
	int lsSize = ls.size();
	for (int i = 0; i < lsSize; ++i) {
		double x, y;
		getPointData(ls[i], x, y);
		if (almost_equal(x, tagX) && almost_equal(y, tagY)) {
			double x1, x2, x3, x4, y1, y2, y3, y4, resX, resY;
			getPointData(ls[(i - 2 + lsSize) % lsSize], x1, y1);
			getPointData(ls[(i - 1 + lsSize) % lsSize], x2, y2);
			getPointData(ls[(i + 1) % lsSize], x3, y3);
			getPointData(ls[(i + 2) % lsSize], x4, y4);

			SegmentLine lineA(x1, y1, x2, y2);
			SegmentLine lineB(x3, y3, x4, y4);

			connectAB(lineA, lineB, resX, resY);

			ls[i].set<0>(resX);
			ls[i].set<1>(resY);

			//makeIt is Close Polygon:
			ls.push_back(ls[0]);
			return;
		}
	}
}

template <typename T>
void startPointRestruct(BoostLineString &originLs, T &ls) {
	double tagX, tagY;
	getPointData(originLs[0], tagX, tagY);

	int startPtCnt = countStartPoint<T>(ls, tagX, tagY);
	while (startPtCnt > 1)
		removeConnectPoint<T>(ls, tagX, tagY), startPtCnt--;

	modifyStartPoint<T>(ls, tagX, tagY);
}

void assemblyBuffer(Polygom &assembly, const double assemblygap, BoostMultiLineString &assemblyMultLine) {
	BoostLineString assemblyLs, resultLs;
	makeLine(assembly, assemblyLs);
	double tagX, tagY;
	getPointData(assemblyLs[0], tagX, tagY);

	//make the outline
	BoostMultipolygon assemblyOutLs;
	bg::strategy::buffer::distance_symmetric<double> assemblygap_dist_strategy(assemblygap);
	boost::geometry::buffer(assemblyLs, assemblyOutLs, assemblygap_dist_strategy, side_strategy, join_strategy, end_strategy, point_strategy);

	assert(assemblyOutLs.size() == 1);
	string strLs = boost::lexical_cast<std::string>(bg::wkt(assemblyOutLs.front()));
	string outLineStrLs = strLs.substr(9, strLs.find("),(") - 9);

	bg::read_wkt("LINESTRING(" + outLineStrLs + ")", resultLs);

	startPointRestruct<BoostLineString>(assemblyLs, resultLs);
	assemblyMultLine.push_back(resultLs);
}

void multiCropperBuffer(BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer) {
	cropperMulLsBuffer.clear();

	BoostMultipolygon cropBuf01, cropBuf02;
	bg::strategy::buffer::distance_symmetric<double> cropper_dist_strategy(croppergap);
	bg::buffer(multiCropperLs, cropBuf01, cropper_dist_strategy, side_strategy, join_strategy, end_strategy, point_strategy);


	for (int i = 0; i < multiCropperLs.size(); ++i) {
		BoostMultipolygon tempBuffered;
		bg::buffer(multiCropperLs[i], tempBuffered, cropper_dist_strategy, side_strategy, join_strategy, end_strategy, point_strategy);

		startPointRestruct<BoostRing>(multiCropperLs[i], tempBuffered[0].outer());
		cropBuf02.push_back(tempBuffered[0]);
	}
	bg::union_(cropBuf01, cropBuf02, cropperMulLsBuffer);
}

void connectLine(vector<BoostLineString> &bgDiff) {
	if (bgDiff.size() == 1) return;
	BoostLineString &stLine = *(bgDiff.begin());
	BoostLineString &edLine = *(bgDiff.end() - 1);

	double stPtX, stPtY, edPtX, edPtY;
	getPointData(stLine[0], stPtX, stPtY);
	getPointData(*(edLine.end() - 1), edPtX, edPtY);

	if (stPtX == edPtX && stPtY == edPtY) {
		for (int i = 0; i < stLine.size(); ++i)
			edLine.push_back(stLine[i]);
		bgDiff.erase(bgDiff.begin());
	}
}

void buildAssemblyLine(Polygom &assembly, const double assemblygap, BoostMultiLineString multiCropperLs, const double croppergap, BoostMultipolygon &cropperMulLsBuffer, vector<BoostLineString> &bgDiff) {
	BoostMultiLineString assemblyMultLine;
	assemblyBuffer(assembly, assemblygap, assemblyMultLine);
	multiCropperBuffer(multiCropperLs, croppergap, cropperMulLsBuffer);

	//difference
	bgDiff.clear();
	bg::difference(assemblyMultLine, cropperMulLsBuffer, bgDiff);
	connectLine(bgDiff);
}
