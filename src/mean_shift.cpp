
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
			group_.create_thread(boost::bind(
						&boost::asio::io_service::run, &io_service_));
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
	float x, y, r, g, b;

	template<typename T> void operator *=(T v);
	void operator +=(const PosColor &v);

	template<typename T> PosColor operator /(T v) const;
	template<typename T> PosColor operator *(T v) const;

	bool operator ==(const PosColor &v) const;
};

template<typename T>
void MeanShifter::PosColor::operator *=(T v) {
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

template<typename T>
MeanShifter::PosColor MeanShifter::PosColor::operator /(T v) const {
	PosColor result;
	result.x = this->x / v;
	result.y = this->y / v;
	result.r = this->r / v;
	result.g = this->g / v;
	result.b = this->b / v;
	return result;
}

template<typename T>
MeanShifter::PosColor MeanShifter::PosColor::operator *(T v) const {
	PosColor result;
	result.x = this->x * v;
	result.y = this->y * v;
	result.r = this->r * v;
	result.g = this->g * v;
	result.b = this->b * v;
	return result;
}

bool MeanShifter::PosColor::operator ==(const MeanShifter::PosColor &v) const {
	return v.x == this->x && v.y == this->y &&
		v.r == this->r && v.g == this->g && v.b == this->b;
}

void MeanShifter::setBandWidth(int Hs, int Hr) {
	Hs_ = Hs;
	Hr_ = Hr;
}

void MeanShifter::setKernel(MeanShifter::Kernel type){
	switch(type){
	case UNIT_BALL:
		profile_position = &MeanShifter::profile_position_unitball;
		profile_color = &MeanShifter::profile_color_unitball;
		break;
	case FUKUNAGA:
		profile_position = &MeanShifter::profile_position_fukunaga;
		profile_color = &MeanShifter::profile_color_fukunaga;
		break;
	case GAUSSIAN:
		profile_position = &MeanShifter::profile_position_gaussian;
		profile_color = &MeanShifter::profile_color_gaussian;
		break;
	}
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
				float r = src.data[index * 3];
				float g = src.data[index * 3 + 1];
				float b = src.data[index * 3 + 2];

				PosColor pre_ = { (float)x, (float)y, r, g, b };
				PosColor res_ = { (float)x, (float)y, r, g, b };

				pool.post([this, index, pre_, res_, iteration, per_100, &src, &dst](){
						PosColor pre = pre_;
						PosColor res = res_;
						float def_x = pre_.x;
						float def_y = pre_.y;
						for (int i = 0; i < iteration; ++i) {
						res = update_cood(res, src);
						if (pre == res)
						break;
						res.x = def_x;
						res.y = def_y;
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
	float total = 0;
	PosColor result = { 0, 0, 0, 0, 0 };

	// 参照ピクセルを中心に前後左右Hsの正方形を探索
	int start_x = p.x - Hs_ < 0 ? 0 : p.x -Hs_;
	int end_x = p.x + Hs_ > src.cols ? src.cols : p.x +Hs_;
	int start_y = p.y - Hs_ < 0 ? 0 : p.y -Hs_;
	int end_y = p.y + Hs_ > src.rows ? src.rows : p.y +Hs_;

	for (int x = start_x; x < end_x; ++x) {
		for (int y = start_y; y < end_y; ++y) {
			int index = y * src.cols + x;
			float r = src.data[index * 3];
			float g = src.data[index * 3 + 1];
			float b = src.data[index * 3 + 2];

			PosColor c = {(float) x, (float)y, r, g, b };
			float k = kernel_position(p.x, p.y, c.x, c.y, Hs_)
				* kernel_color(p.r, p.g, p.b, c.r, c.g, c.b, Hr_);

			result += c * k;
			total += k;
		}
	}
	return result / total;
}

float MeanShifter::kernel_position(
		int ref_x, int ref_y, int x, int y, int h) {
	return profile_position(sqrt(
				pow((float) (ref_x - x), 2.0f) +
				pow((float) (ref_y - y), 2.0f))
			/ (float) h);
}

float MeanShifter::kernel_color(
		int ref_r, int ref_g, int ref_b, int r, int g, int b, int h) {
	return profile_color(sqrt(
				pow((float) (ref_r - r), 2.0f) +
				pow((float) (ref_g - g), 2.0f) +
				pow((float) (ref_b - b), 2.0f))
			/ (float) h);
}

float MeanShifter::profile_position_fukunaga(float val){
	return val > 1 ? 0 : 1;
}
float MeanShifter::profile_color_fukunaga(float val){
	return val > 1 ? 0 : 1;
}

float MeanShifter::profile_position_unitball(float val){
	return val > 1 ? 0 : 1.0f-val;
}
float MeanShifter::profile_color_unitball(float val){
	return val > 1 ? 0 : 1.0f-val;
}

float MeanShifter::profile_position_gaussian(float val){
	return exp(-val/2.0f);
}
float MeanShifter::profile_color_gaussian(float val){
	return exp(-val/2.0f);
}
