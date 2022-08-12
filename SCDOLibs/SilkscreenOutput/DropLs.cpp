#include "DropLs.h"

void dropLs(vector<bool> &canWrite, vector<SilkSet> &skSt) {
	canWrite.resize(skSt.size());
	fill(canWrite.begin(), canWrite.end(), true);

	double max_x = -9999.0, min_x = 9999.0, max_y = -9999.0, min_y = 9999.0;


	for (int i = 0; i < skSt.size(); ++i) {
		max_x = max(max_x, skSt[i].max_x), max_y = max(max_y, skSt[i].max_y);
		min_x = min(min_x, skSt[i].min_x), min_y = min(min_y, skSt[i].min_y);
	}

	int max_x_cnt = 0, max_y_cnt = 0, min_x_cnt = 0, min_y_cnt = 0;
	for (int i = 0; i < skSt.size(); ++i) {
		cntMaxMin(max_x, max_y, min_x, min_y, max_x_cnt, max_y_cnt, min_x_cnt, min_y_cnt, skSt[i]);
	}

	sort(skSt.begin(), skSt.end());

	for (int i = 0; i < skSt.size(); ++i) {
		int cur_max_x = 0, cur_max_y = 0, cur_min_x = 0, cur_min_y = 0;
		cntMaxMin(max_x, max_y, min_x, min_y, cur_max_x, cur_max_y, cur_min_x, cur_min_y, skSt[i]);

		if (max_x_cnt - cur_max_x > 0 &&
			max_y_cnt - cur_max_y > 0 &&
			min_x_cnt - cur_min_x > 0 &&
			min_y_cnt - cur_min_y > 0) {

			max_x_cnt -= cur_max_x;
			max_y_cnt -= cur_max_y;
			min_x_cnt -= cur_min_x;
			min_y_cnt -= cur_min_y;
			canWrite[i] = false;
		}
	}
}

void cntMaxMin(double max_x, double max_y, double min_x, double min_y, int &max_x_cnt, int &max_y_cnt, int &min_x_cnt, int &min_y_cnt, SilkSet &skSt) {
	isCoordEquals(max_x, skSt.max_x, max_x_cnt);
	isCoordEquals(max_y, skSt.max_y, max_y_cnt);
	isCoordEquals(min_x, skSt.min_x, min_x_cnt);
	isCoordEquals(min_y, skSt.min_y, min_y_cnt);
}


void isCoordEquals(double max_cr, double cur_max, int &cnt) {
	if (almost_equal(max_cr, cur_max)) cnt++;
}
