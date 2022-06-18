#include "GraphDraw.h"

void checkoutPutResult(string fName, Polygom &assembly, BoostPolygon &bgAssembly, BoostMultipolygon &multBGCropper, BoostMultipolygon &cropperMulLsBuffer) {
	string str;
	std::ifstream input(fName + ".txt");
	vector<BoostLineString> v_ls;
	BoostLineString ls;
	Polygom outline;

	//assembly
	input >> str;
	while (input >> str) {
		if (str[0] == 's') {
			makeLine(outline, ls);
			v_ls.push_back(ls);
			outline.shape = "";
			continue;
		}
		outline.addLine(str);
	}
	makeLine(outline, ls);
	v_ls.push_back(ls);
	input.close();


	BoostPoint pt(-1.0, 0.0);
	{
		std::ofstream svg(fName + "Check.svg");
		boost::geometry::svg_mapper<BoostPoint> mapper(svg, 400, 400);

		mapper.add(bgAssembly);
		mapper.add(multBGCropper);
		//mapper.add(cropperMulLsBuffer);
		mapper.add(pt);

		for (int i = 0; i < v_ls.size(); ++i) {
			mapper.add(v_ls[i]);
			mapper.map(v_ls[i], "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
		}

		mapper.map(bgAssembly, "fill-opacity:0.5;fill:rgb(51,153,255);stroke:rgb(0,77,153);stroke-width:2");
		mapper.map(multBGCropper, "fill-opacity:0.5;fill:rgb(204,153,0);stroke:rgb(202,153,0);stroke-width:2");
		//mapper.map(cropperMulLsBuffer, "fill-opacity:0.5;fill:rgb(255, 255, 153);stroke:rgb(77, 77, 0);stroke-width:2");
		mapper.map(pt, "fill-opacity:0.5;fill:rgb(0,0,0);stroke:rgb(0,0,0);stroke-width:2");
	}
}

void resultSample(string fName, Polygom assembly, BoostPolygon &bgAssembly, BoostMultipolygon &multBGCropper, BoostMultipolygon &cropperMulLsBuffer, vector<BoostPolygon> &cycleList, vector<BoostLineString> &bgDiff, double silkscreenlen) {
	BoostPoint pt(-0.1, 0.0);
	
	{
		std::ofstream svg(fName + ".svg");
		boost::geometry::svg_mapper<BoostPoint> mapper(svg, 400, 400);

		mapper.add(bgAssembly);
		mapper.add(multBGCropper);
		mapper.add(cropperMulLsBuffer);
		mapper.add(pt);


		for (int i = 0; i < cycleList.size(); ++i) {
			mapper.add(cycleList[i]);
			mapper.map(cycleList[i], "fill-opacity:0.5;fill:rgb(255, 102, 255);stroke:rgb(102, 0, 102);stroke-width:2");
		}


		for (int i = 0; i < bgDiff.size(); ++i) {
			if (bg::within(bgDiff[i], multBGCropper) == false && isLargerEnough(bgDiff[i], silkscreenlen)) {
				mapper.add(bgDiff[i]);
				mapper.map(bgDiff[i], "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:2");
			}
		}

		mapper.map(bgAssembly, "fill-opacity:0.5;fill:rgb(51,153,255);stroke:rgb(0,77,153);stroke-width:2");
		mapper.map(multBGCropper, "fill-opacity:0.5;fill:rgb(204,153,0);stroke:rgb(202,153,0);stroke-width:2");
		mapper.map(cropperMulLsBuffer, "fill-opacity:0.5;fill:rgb(255, 255, 153);stroke:rgb(77, 77, 0);stroke-width:2");
		mapper.map(pt, "fill-opacity:0.5;fill:rgb(0,0,0);stroke:rgb(0,0,0);stroke-width:2");

	}
}

