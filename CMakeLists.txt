CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(MeanShiftFilter CXX)
ADD_EXECUTABLE(mshift src/mshift.cpp src/mean_shift.cpp)
TARGET_LINK_LIBRARIES(mshift opencv_core opencv_highgui opencv_imgproc boost_thread boost_timer boost_system)

# リリースビルド用オプション
SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3 -std=c++0x")
# デバッグビルド用オプション
SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -std=c++0x")
# ビルドのタイプはリリース
SET(CMAKE_BUILD_TYPE Release)

