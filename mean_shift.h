
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

class MeanShifter {
	public:
		MeanShifter(){}
		~MeanShifter(){}

		void setBandWidth(int Hs, int Hr);
		void perform(const cv::Mat &src, cv::Mat &dst, int iteration);

	private:
		int Hs_;
		int Hr_;	

		struct PosColor{
			int x, y;
			unsigned char r, g, b;

			void operator *=(int v){
				x *=v;
				y *=v;
				r *=v;
				g *=v;
				b *=v;
			}
			void operator +=(const PosColor &v){
				x +=v.x;
				y +=v.y;
				r +=v.r;
				g +=v.g;
				b +=v.b;
			}
			PosColor operator /(int v) const {
				PosColor result;
				result.x =this->x /v;
				result.y =this->y /v;
				result.r =this->r /v;
				result.g =this->g /v;
				result.b =this->b /v;
				return result;
			}
			PosColor operator *(int v) const {
				PosColor result;
				result.x =this->x *v;
				result.y =this->y *v;
				result.r =this->r *v;
				result.g =this->g *v;
				result.b =this->b *v;
				return result;
			}
		};

		PosColor update_cood(const PosColor &p, const cv::Mat &src);

		int kernel_position(int ref_x, int ref_y, int x, int y, int h);
		int kernel_color(int ref_r, int ref_g, int ref_b, int r, int g, int b, int h);

		int profile_position(int val);
		int profile_color(int val);
};
