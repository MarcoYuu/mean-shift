#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <cmath>

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

#include <boost/lexical_cast.hpp>

#include "mean_shift.h"

using namespace std;
using namespace cv;
using namespace boost;

int main(int argc, char *argv[]){

	if(argc != 5){
		cout << "arg error" <<endl;
		return 0;
	}
	string file_name = argv[1];
	int Hs = lexical_cast<int>(argv[2]);
	int Hr = lexical_cast<int>(argv[3]);
	int iterate = lexical_cast<int>(argv[4]);

	const char* WIN_SRC = "Source";
	const char* WIN_DST = "Result";

	Mat src = imread(file_name);
	Mat dst(src.rows, src.cols, CV_8UC3);

	MeanShifter shifter;
	shifter.setBandWidth(Hs, Hr);
	shifter.perform(src, dst, iterate);

	// Windows
	//namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
	//namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
	//cvMoveWindow(WIN_SRC, 400     , 0);
	//cvMoveWindow(WIN_DST, src.cols, 0);

	//imshow(WIN_SRC, src);
	//imshow(WIN_DST, dst);

	//waitKey(0);

	cv::imwrite("test.png", dst, std::vector<int>({CV_IMWRITE_PNG_COMPRESSION, 1}));

	return 0;
}
