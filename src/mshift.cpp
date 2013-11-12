#include <iostream> // for standard I/O
#include <string>   // for strings
#include <cmath>

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur

#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <boost/format.hpp>

#include "mean_shift.h"

using namespace std;
using namespace cv;
using namespace boost;

int main(int argc, char *argv[]){

	// MeanShiftの設定
	if(argc < 6){
		cout << "arg error" <<endl;
		return 0;
	}
	string file_name = argv[1];
	int Hs = lexical_cast<int>(argv[2]);
	int Hr = lexical_cast<int>(argv[3]);
	int iterate = lexical_cast<int>(argv[4]);
	int thread = lexical_cast<int>(argv[5]);
	MeanShifter::Kernel kernel = MeanShifter::FUKUNAGA;
	if(argc == 7){
		string k_type = argv[6];
		cout << k_type << endl;
		if(k_type == "fukunaga")
			kernel = MeanShifter::FUKUNAGA;
		else if(k_type == "unitball")
			kernel = MeanShifter::UNIT_BALL;
		else if(k_type == "gaussian")
			kernel = MeanShifter::GAUSSIAN;
		cout << "kernel: " << kernel << endl;
	}

	// 画像の読み込み
	Mat src = imread(file_name);
	Mat dst(src.rows, src.cols, CV_8UC3);

	// MeanShift
	MeanShifter shifter;
	shifter.setBandWidth(Hs, Hr);
	shifter.setKernel(kernel);

	timer::cpu_timer timer;
	shifter.perform(src, dst, iterate, thread);
	timer.stop();

	cout << timer.format() <<endl;

	// 出力
	cv::imwrite((boost::format("%d_s%d_r%d_i%d_k%d_th%d.png")
				% src.cols % Hs % Hr % iterate % kernel % thread).str(),
			dst, std::vector<int>({CV_IMWRITE_PNG_COMPRESSION, 1}));

	return 0;
}
