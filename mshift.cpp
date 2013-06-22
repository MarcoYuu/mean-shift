#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <cmath>  // string to number conversion

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

#include "mean_shift.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[]){

	if(argc != 2){
		cout << "arg error" <<endl;
		return 0;
	}
	string file_name = argv[1];

	const char* WIN_SRC = "Source";
	const char* WIN_DST = "Result";

	Mat src = imread(file_name);
	Mat dst(src.rows, src.cols, CV_8UC3);

	for (int x = 0; x < src.cols; ++x) {
		for (int y = 0; y < src.rows; ++y) {
			int index = y*src.cols+x;
			dst.data[index*3] =dst.data[index*3+1] =dst.data[index*3+2] =
				(77*src.data[index*3]+150*src.data[index*3+1]+29*src.data[index*3+2]) >> 8;
		}
	}

	// Windows
	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
	namedWindow(WIN_DST, CV_WINDOW_AUTOSIZE);
	cvMoveWindow(WIN_SRC, 400     , 0);
	cvMoveWindow(WIN_DST, src.cols, 0);

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);

	cv::imwrite("test.png", dst, std::vector<int>({CV_IMWRITE_PNG_COMPRESSION, 1}));

	return 0;
}
