
#include <iostream> // for standard I/O
#include <cmath>
#include "mean_shift.h"

using namespace std;
using namespace cv;

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

// http://d.hatena.ne.jp/faith_and_brave/20110408/1302248501
class thread_pool {
	private:
	boost::asio::io_service& io_service_;
	boost::shared_ptr<boost::asio::io_service::work> work_;
	boost::thread_group group_;

	public:
	thread_pool(boost::asio::io_service& io_service, std::size_t size)
		: io_service_(io_service)
	{
		work_.reset(new boost::asio::io_service::work(io_service_));

		for (std::size_t i = 0; i < size; ++i) {
			group_.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));
		}
	}

	~thread_pool(){
		work_.reset();
		group_.join_all();
	}

	template <class F> void post(F f){
		io_service_.post(f);
	}
};

struct MeanShifter::PosColor {
	int x, y, r, g, b;

	void operator *=(int v);
	void operator +=(const PosColor &v);

	PosColor operator /(int v) const;
	PosColor operator *(int v) const;

	bool operator ==(const PosColor &v) const;
};

void MeanShifter::PosColor::operator *=(int v) {
	x *= v;
	y *= v;
	r *= v;
	g *= v;
	b *= v;
}

void MeanShifter::PosColor::operator +=(const MeanShifter::PosColor &v) {
	x += v.x;
	y += v.y;
	r += v.r;
	g += v.g;
	b += v.b;
}
 
MeanShifter::PosColor MeanShifter::PosColor::operator /(int v) const {
	PosColor result;
	result.x = this->x / v;
	result.y = this->y / v;
	result.r = this->r / v;
	result.g = this->g / v;
	result.b = this->b / v;
	return result;
}

MeanShifter::PosColor MeanShifter::PosColor::operator *(int v) const {
	PosColor result;
	result.x = this->x * v;
	result.y = this->y * v;
	result.r = this->r * v;
	result.g = this->g * v;
	result.b = this->b * v;
	return result;
}

bool MeanShifter::PosColor::operator ==(const MeanShifter::PosColor &v) const {
	return v.x == this->x && v.y == this->y && v.r == this->r && v.g == this->g && v.b == this->b;
}

void MeanShifter::setBandWidth(int Hs, int Hr) {
	Hs_ = Hs;
	Hr_ = Hr;
}

void MeanShifter::perform(const Mat &src, Mat &dst, int iteration, int thread) {
	int per_100 = src.cols*src.rows/100;
	cout <<"[" << flush;
	{
		boost::asio::io_service io_service;
		thread_pool pool(io_service, thread);

		for (int x = 0; x < src.cols; ++x) {
			for (int y = 0; y < src.rows; ++y) {
				int index = y * src.cols + x;
				unsigned char r = src.data[index * 3];
				unsigned char g = src.data[index * 3 + 1];
				unsigned char b = src.data[index * 3 + 2];

				PosColor pre_ = { x, y, r, g, b };
				PosColor res_ = { x, y, r, g, b };

				pool.post([this, index, pre_, res_, iteration, per_100, &src, &dst](){
   					PosColor pre = pre_;
   					PosColor res = res_;
   					for (int i = 0; i < iteration; ++i) {
   						res = update_cood(res, src);
   						if (pre == res)
   							break;
   						pre = res;
   					}

   					dst.data[index * 3 + 0] = res.r;
   					dst.data[index * 3 + 1] = res.g;
   					dst.data[index * 3 + 2] = res.b;

   					if(index%per_100 == 0)
   						cout << "=" << flush;
   				});
			}
		}
	}
	cout <<"]" <<endl;
}

MeanShifter::PosColor MeanShifter::update_cood(const PosColor &p, const Mat &src) {
	int total = 0;
	PosColor result = { 0, 0, 0, 0, 0 };

	// 参照ピクセルを中心に前後左右Hsの正方形を探索
	int start_x = p.x - Hs_ < 0 ? 0 : p.x -Hs_;
	int end_x = p.x + Hs_ > src.cols ? src.cols : p.x +Hs_;
	int start_y = p.y - Hs_ < 0 ? 0 : p.y -Hs_;
	int end_y = p.y + Hs_ > src.cols ? src.cols : p.y +Hs_;

	for (int x = start_x; x < end_x; ++x) {
		for (int y = start_y; y < end_y; ++y) {
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
