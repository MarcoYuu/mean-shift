
#include <iostream> // for standard I/O
#include <cmath>
#include "mean_shift.h"
#include <boost/thread.hpp>

using namespace std;
using namespace cv;

void MeanShifter::setBandWidth(int Hs, int Hr) {
	Hs_ = Hs;
	Hr_ = Hr;
}

void MeanShifter::perform(const Mat &src, Mat &dst, int iteration) {
	int per_100 = src.cols*src.rows/100;
	cout <<"[" << flush;
	for (int x = 0; x < src.cols; ++x) {
		for (int y = 0; y < src.rows; ++y) {
			int index = y * src.cols + x;
			unsigned char r = src.data[index * 3];
			unsigned char g = src.data[index * 3 + 1];
			unsigned char b = src.data[index * 3 + 2];

			PosColor pre = { x, y, r, g, b };
			PosColor res = { x, y, r, g, b };
			for (int i = 0; i < iteration; ++i) {
				res = update_cood(res, src);
				if (pre == res)
					break;
				pre = res;
			}

			dst.data[index * 3] = res.r;
			dst.data[index * 3 + 1] = res.g;
			dst.data[index * 3 + 2] = res.b;

			if(index%per_100 == 0)
				cout << "=" << flush;
		}
	}
	cout <<"]" <<endl;
}

MeanShifter::PosColor MeanShifter::update_cood(const PosColor &p, const Mat &src) {
	int total = 0;
	PosColor result = { 0, 0, 0, 0, 0 };
	for (int x = 0; x < src.cols; ++x) {
		for (int y = 0; y < src.rows; ++y) {
			int index = y * src.cols + x;
			unsigned char r = src.data[index * 3];
			unsigned char g = src.data[index * 3 + 1];
			unsigned char b = src.data[index * 3 + 2];

			PosColor c = { x, y, r, g, b };
			int k = kernel_position(p.x, p.y, c.x, c.y, Hs_)
				* kernel_color(p.r, p.g, p.b, c.r, c.g, c.b, Hr_);

			result += c * k;
			total += k;
		}
	}
	return result / total;
}

int MeanShifter::kernel_position(int ref_x, int ref_y, int x, int y, int h) {
	return profile_position(sqrt(pow((float) (ref_x - x), 2) + pow((float) (ref_y - y), 2)) / (float) h);
}
int MeanShifter::kernel_color(int ref_r, int ref_g, int ref_b, int r, int g, int b, int h) {
	return profile_color(
			sqrt(pow((float) (ref_r - r), 2) + pow((float) (ref_g - g), 2) + pow((float) (ref_b - b), 2))
			/ (float) h);
}

int MeanShifter::profile_position(int val) {
	return val > 1 ? 0 : 1;
}
int MeanShifter::profile_color(int val) {
	return val > 1 ? 0 : 1;
}
