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

double calcMse(Mat& im1, Mat& im2) {
	double ans = 0;
	int height = im1.rows;
	int width = im2.cols;
	int N = height * width;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			double value1 = im1.at<double>(x, y);
			double value2 = im2.at<double>(x, y);
			double value = value1 - value2;
			ans += value * value;
		}
	}
	ans /= N;
	return ans;
}

double calcPsnr(Mat& im1, Mat& im2) {
	double M = 255;
	double MSE = calcMse(im1, im2);
	if (MSE == 0) {
		cout << "psnr:   " << "Identical pictures!!!" << endl;
		return 10.0 * log10(M * M);
	}
	else {
		return 10.0 * log10(M * M / MSE);
	}
}

double CalcMod(double a, int b) {
	return a - floor(a / b) * b;
}

Mat bgrToHsv(Mat& imBGR) {
	Mat imHSV;
	imBGR.copyTo(imHSV);
	int height = imBGR.rows;
	int width = imBGR.cols;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			int b = imBGR.at<Vec3b>(x, y)[0];
			int g = imBGR.at<Vec3b>(x, y)[1];
			int r = imBGR.at<Vec3b>(x, y)[2];
			double bb = (double)b / 255;
			double gg = (double)g / 255;
			double rr = (double)r / 255;
			double Cmax = max({ bb, gg, rr });
			double Cmin = min({ bb, gg, rr });
			double d = Cmax - Cmin;
			double H, S, V;
			double qwe = CalcMod((gg - bb) / d, 6);
			if (d == 0) H = 0;
			else if (Cmax == rr) H = 60 * qwe;
			else if (Cmax == gg) H = 60 * (((bb - rr) / d) + 2);
			else if (Cmax == bb) H = 60 * (((rr - gg) / d) + 4);
			if (Cmax == 0) S = 0;
			else S = d / Cmax;
			V = Cmax;
			Vec3d kek = Vec3d(H, S * 255, V * 255);
			imHSV.at<Vec3b>(x, y) = kek;
		}
	}
	return imHSV;
}

Mat hsvToBgr(Mat& imHSV) {
	Mat imBGR;
	imHSV.copyTo(imBGR);
	int height = imHSV.rows;
	int width = imHSV.cols;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			double H = imHSV.at<Vec3b>(x, y)[0];
			double S = (double)imHSV.at<Vec3b>(x, y)[1] / 255;
			double V = (double)imHSV.at<Vec3b>(x, y)[2] / 255;
			double C = V * S;
			double X = C * (1 - abs(CalcMod(H / 60, 2) - 1));
			double m = V - C;
			double h = H;
			Vec3d kek;
			if (h >= 0 && h < 60)		kek = Vec3d(0, X, C);
			if (h >= 60 && h < 120)		kek = Vec3d(0, C, X);
			if (h >= 120 && h < 180)	kek = Vec3d(X, C, 0);
			if (h >= 180 && h < 240)	kek = Vec3d(C, X, 0);
			if (h >= 240 && h < 300)	kek = Vec3d(C, 0, X);
			if (h >= 300 && h < 360)	kek = Vec3d(X, 0, C);
			kek[0] = (kek[0] + m) * 255;
			kek[1] = (kek[1] + m) * 255;
			kek[2] = (kek[2] + m) * 255;

			imBGR.at<Vec3b>(x, y) = kek;
		}
	}
	return imBGR;
}

Mat changeBrightness(Mat& input, double val) {
	Mat output;
	input.copyTo(output);
	int height = input.rows;
	int width = input.cols;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			output.at<Vec3b>(x, y) = input.at<Vec3b>(x, y) * val;
		}
	}
	return output;
}

int main() {
	//Метрика сходства двух изображений
	String im_name1("../data/1.jpg");
	String im_name2("../data/2.jpg");
	Mat img1 = imread(im_name1);
	Mat img2 = imread(im_name2);
	imshow("IMG1", img1);
	waitKey();
	imshow("IMG2", img2);
	waitKey();
	img1.convertTo(img1, CV_64F);
	img2.convertTo(img2, CV_64F);
	int height1 = img1.rows;
	int width1 = img1.cols;
	int height2 = img2.rows;
	int width2 = img2.cols;

	if (height1 != height2 || width1 != width2)cout << -1 << endl;
	else {
		cout << "psnr:   " << calcPsnr(img1, img2) << endl;
	}

	//Конвертация цветного изображения в монохромное изображение
	String im_name3("../data/4.jpg");
	Mat tmp = imread(im_name3);
	Mat img_gray;
	cvtColor(tmp, img_gray, COLOR_BGR2GRAY);
	imshow("IMG_GRAY", img_gray);
	waitKey();

	String im_name4("../data/4.jpg");
	Mat imgToGray = imread(im_name4);
	int height = imgToGray.rows;
	int width = imgToGray.cols;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			int m = imgToGray.at<Vec3b>(x, y)[0] + imgToGray.at<Vec3b>(x, y)[1] + imgToGray.at<Vec3b>(x, y)[2];
			m /= 3;
			imgToGray.at<Vec3b>(x, y) = Vec3b(m, m, m);
		}
	}
	imshow("MY_IMG_GRAY", imgToGray);
	waitKey();


	//BGR <---> HSV
	String im_name5("../data/1.jpg");
	Mat imgBGR = imread(im_name5);
	Mat imBGR2HSV, imHSV2BGR;
	cvtColor(imgBGR, imBGR2HSV, COLOR_BGR2HSV); //BGR --->HSV
	cvtColor(imBGR2HSV, imHSV2BGR, COLOR_HSV2BGR);//HSV ---> BGR
	imshow("BGR to HSV", imBGR2HSV);
	waitKey();
	imshow("HSV to BGR", imHSV2BGR);
	waitKey();

	String im_name6("../data/1.jpg");
	Mat imBGR = imread(im_name6);
	Mat imHSV = bgrToHsv(imBGR);//BGR --->HSV
	imshow("MY BGR to HSV", imHSV);
	waitKey();
	Mat kek = hsvToBgr(imHSV);//HSV ---> BGR
	imshow("MY HSV to BGR", kek);
	waitKey();

	//увеличение яркости
	String im_name7("../data/1.jpg");
	Mat image = imread(im_name6);
	Mat imageOutput = changeBrightness(image, 1.5);
	imshow("My change brightness of RGB", imageOutput);
	waitKey();

	String im_name8("../data/1.jpg");
	image = imread(im_name6);
	Mat imageHSV = bgrToHsv(image);
	imageOutput = changeBrightness(imageHSV, 1.5);
	imshow("My change brightness of HSV", imageOutput);
	waitKey();
	return 0;
}