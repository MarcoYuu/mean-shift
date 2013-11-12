#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

class MeanShifter {
public:
	MeanShifter() :Hs_(8), Hr_(16) {
		profile_position = &MeanShifter::profile_position_fukunaga;
		profile_color = &MeanShifter::profile_color_fukunaga;
	}
	~MeanShifter() {
	}

	enum Kernel{
		UNIT_BALL,
		FUKUNAGA,
		GAUSSIAN
	};

	void setBandWidth(int Hs, int Hr);
	void setKernel(Kernel type);
	void perform(const cv::Mat &src, cv::Mat &dst, int iteration, int thread);

private:
	int Hs_;
	int Hr_;

	struct PosColor;
	PosColor update_cood(const PosColor &p, const cv::Mat &src);

	float kernel_position(int ref_x, int ref_y, int x, int y, int h);
	float kernel_color(int ref_r, int ref_g, int ref_b, int r, int g, int b, int h);

	float (*profile_position)(float val);
	float (*profile_color)(float val);

	static float profile_position_fukunaga(float val);
	static float profile_color_fukunaga(float val);

	static float profile_position_unitball(float val);
	static float profile_color_unitball(float val);

	static float profile_position_gaussian(float val);
	static float profile_color_gaussian(float val);
};
