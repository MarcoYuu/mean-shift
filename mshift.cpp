#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <cmath>

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>

#include "mean_shift.h"

using namespace std;
using namespace cv;
using namespace boost;

int main(int argc, char *argv[]){

	if(argc != 6){
		cout << "arg error" <<endl;
		return 0;
	}
	string file_name = argv[1];
	int Hs = lexical_cast<int>(argv[2]);
	int Hr = lexical_cast<int>(argv[3]);
	int iterate = lexical_cast<int>(argv[4]);
	int thread = lexical_cast<int>(argv[5]);

	Mat src = imread(file_name);
	Mat dst(src.rows, src.cols, CV_8UC3);

	MeanShifter shifter;
	shifter.setBandWidth(Hs, Hr);

	timer::cpu_timer timer;	
	shifter.perform(src, dst, iterate, thread);
	timer.stop();
	cout << timer.format() <<endl;

	cv::imwrite("test.png", dst, std::vector<int>({CV_IMWRITE_PNG_COMPRESSION, 1}));

	return 0;
}
