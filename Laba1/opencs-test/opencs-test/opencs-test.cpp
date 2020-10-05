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
	ans += sum((im1 - im2).mul(im1 - im2))[0];
	ans += sum((im1 - im2).mul(im1 - im2))[1];
	ans += sum((im1 - im2).mul(im1 - im2))[2];
	return ans / N;
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
	imBGR.forEach<Vec3b>([](Vec3b& p, const int* pos) {
		int b = p[0];
		int g = p[1];
		int r = p[2];
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
		Vec3d hsv = Vec3d(H, S * 255.0, V * 255.0);
		p = hsv;

		});
	return imBGR;
}

Mat hsvToBgr(Mat& imHSV) {
	imHSV.forEach<Vec3b>([](Vec3b& p, const int* pos) {
		double H = p[0];
		double S = p[1] / 255.0;
		double V = p[2] / 255.0;
		double C = V * S;
		double X = C * (1 - abs(CalcMod(H / 60, 2) - 1));
		double m = V - C;
		double h = H;
		Vec3d bgr;
		if (h >= 0 && h < 60)		bgr = Vec3d(0, X, C);
		if (h >= 60 && h < 120)		bgr = Vec3d(0, C, X);
		if (h >= 120 && h < 180)	bgr = Vec3d(X, C, 0);
		if (h >= 180 && h < 240)	bgr = Vec3d(C, X, 0);
		if (h >= 240 && h < 300)	bgr = Vec3d(C, 0, X);
		if (h >= 300 && h < 360)	bgr = Vec3d(X, 0, C);
		bgr[0] = (bgr[0] + m) * 255;
		bgr[1] = (bgr[1] + m) * 255;
		bgr[2] = (bgr[2] + m) * 255;
		p = bgr;
		});
	return imHSV;
}


Mat changeBrightness(Mat& input, double val) {
	return input * val;
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
		long double T1 = clock();
		double PSNR = calcPsnr(img1, img2);
		long double T2 = clock();
		T2 -= T1;
		cout << "psnr:   " << PSNR << endl;
		cout << "psnr time :  " << fixed << setprecision(20) << T2 / CLOCKS_PER_SEC << '\n';
	}

	//Конвертация цветного изображения в монохромное изображение
	String im_name3("../data/4.jpg");
	Mat tmp = imread(im_name3);
	Mat img_gray;
	long double t1gray = clock();
	cvtColor(tmp, img_gray, COLOR_BGR2GRAY);
	long double t2gray = clock();
	t2gray -= t1gray;
	cout << "opencv BGR to GRAY:  " << fixed << setprecision(20) << t2gray / CLOCKS_PER_SEC << '\n';
	imshow("IMG_GRAY", img_gray);
	waitKey();

	String im_name4("../data/4.jpg");
	Mat imgToGray = imread(im_name4);
	long double t1gray1 = clock();
	imgToGray.forEach<Vec3b>([](Vec3b& p, const int* pos) {
		int m = p[0] + p[1] + p[2];
		m /= 3;
		p[0] = m;
		p[1] = m;
		p[2] = m;

		});
	long double t2gray1 = clock();
	t2gray1 -= t1gray1;
	cout << "MY BGR to GRAY:  " << fixed << setprecision(20) << t2gray1 / CLOCKS_PER_SEC << '\n';
	imshow("MY_IMG_GRAY", imgToGray);
	waitKey();


	//BGR <---> HSV
	String im_name5("../data/1.jpg");
	Mat imgBGR = imread(im_name5);
	Mat imBGR2HSV, imHSV2BGR;
	long double t1bgr = clock();
	cvtColor(imgBGR, imBGR2HSV, COLOR_BGR2HSV); //BGR --->HSV
	long double t2bgr = clock();
	t2bgr -= t1bgr;
	cout << "opencv BGR to HSV:  " << fixed << setprecision(20) << t2bgr / CLOCKS_PER_SEC << '\n';
	long double t1hsv = clock();
	cvtColor(imBGR2HSV, imHSV2BGR, COLOR_HSV2BGR);//HSV ---> BGR
	long double t2hsv = clock();
	t2hsv -= t1hsv;
	cout << "opencv HSV to BGR:  " << fixed << setprecision(20) << t2hsv / CLOCKS_PER_SEC << '\n';
	imshow("BGR to HSV", imBGR2HSV);
	waitKey();
	imshow("HSV to BGR", imHSV2BGR);
	waitKey();

	String im_name6("../data/1.jpg");
	Mat imBGR = imread(im_name6);
	long double t1bgrToHsv = clock();
	Mat imHSV = bgrToHsv(imBGR);//BGR --->HSV
	long double t2bgrToHsv = clock();
	t2bgrToHsv -= t1bgrToHsv;
	cout << "MY BGR to HSV:  " << fixed << setprecision(20) << t2bgrToHsv / CLOCKS_PER_SEC << '\n';
	imshow("MY BGR to HSV", imHSV);
	waitKey();
	long double t1hsvToBgr = clock();
	Mat toBgr = hsvToBgr(imHSV);//HSV ---> BGR
	long double t2hsvToBgr = clock();
	t2hsvToBgr -= t1hsvToBgr;
	cout << "MY HSV to BGR:  " << fixed << setprecision(20) << t2hsvToBgr / CLOCKS_PER_SEC << '\n';
	imshow("MY HSV to BGR", toBgr);
	waitKey();

	//увеличение яркости
	String im_name7("../data/1.jpg");
	Mat image = imread(im_name7);
	long double t1 = clock();
	Mat imageOutput = changeBrightness(image, 1.5);
	long double t2 = clock();
	t2 -= t1;
	cout << "My change brightness of RGB:  " << fixed << setprecision(20) << t2 / CLOCKS_PER_SEC << '\n';
	imshow("My change brightness of RGB", imageOutput);
	waitKey();

	String im_name8("../data/1.jpg");
	image = imread(im_name6);
	Mat imageHSV = bgrToHsv(image);
	long double t1HSV = clock();
	imageOutput = changeBrightness(imageHSV, 1.5);
	long double t2HSV = clock();
	t2HSV -= t1HSV;
	cout << "My change brightness of HSV:  " << fixed << setprecision(20) << t2HSV / CLOCKS_PER_SEC << '\n';
	imshow("My change brightness of HSV", imageOutput);
	waitKey();
	return 0;
}