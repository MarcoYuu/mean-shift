#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

class MeanShifter {
	public:
		MeanShifter() :Hs_(8), Hr_(16) {
		}
		~MeanShifter() {
		}

		void setBandWidth(int Hs, int Hr);
		void perform(const cv::Mat &src, cv::Mat &dst, int iteration, int thread);

	private:
		int Hs_;
		int Hr_;

		struct PosColor;
		PosColor update_cood(const PosColor &p, const cv::Mat &src);

		int kernel_position(int ref_x, int ref_y, int x, int y, int h);
		int kernel_color(int ref_r, int ref_g, int ref_b, int r, int g, int b, int h);

		int profile_position(int val);
		int profile_color(int val);
};
