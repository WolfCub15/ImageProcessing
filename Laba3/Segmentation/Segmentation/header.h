#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/core/utils/filesystem.hpp>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include<vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
using namespace cv::utils::fs;

Mat toGrayScale(Mat& im);
Mat Gauss(Mat& img_in, int radius, float sigma);
Mat Sobel(Mat im);
Mat nonMax(Mat im);
Mat threshold(Mat im, int a, int b);
Mat watershed(Mat im);
void DistanceTransform(InputArray _src,
    OutputArray _dst,
    int distType,
    int maskSize);


