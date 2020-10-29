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

template <class T>
T check(T v, int max, int min) {
	if (v > max) return max;
	else if (v < min) return min;
	else return v;
}

Mat median(const Mat& in, int radius) {
	Mat result(in);
	for (int x = 0; x < in.rows; ++x) {
		for (int y = 0; y < in.cols; ++y) {
			int size = 2 * radius + 1;
			int Size = size * size;
			vector<float> ansR(Size),ansG(Size),ansB(Size);
			for (int i = -radius; i <= radius; i++) {
				for (int j = -radius; j <= radius; j++) {
					int idx = (i + radius) * size + j + radius;
					ansB[idx] = result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[0];
					ansG[idx] = result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[1];
					ansR[idx] = result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[2];
				}
			}
			sort(ansB.begin(), ansB.end());
			sort(ansG.begin(), ansG.end());
			sort(ansR.begin(), ansR.end());
			int position = Size >> 1;
			result.at<Vec3b>(x, y) = Vec3b(check(ansB[position],255,0), check(ansG[position],255,0), check(ansR[position],255,0));
		}
	}
	return result;
}

Mat gaussian(const Mat& in, int radius,float sigma) {
	Mat result(in);
	int size = 2 * radius + 1;//размер ядра
	float normir = 0;//коэффициент нормировки ядра
	vector<float>vec(size * size);//ядро фильтра
	for (int i = -radius; i <= radius; i++) {//ядро линейного фильтра
		for (int j = -radius; j <= radius; j++) {
			int idx = (i + radius) * size + (j + radius);
			vec[idx] = exp(-(i * i + j * j) / (2*sigma * sigma));
			vec[idx] /=(2 * 3.14 * sigma * sigma);
			normir += vec[idx];
		}
	}
	for (int i = 0; i < size; i++) {//нормируем ядро
		for (int j = 0; j < size; j++) {
			int n = i * size + j;
			vec[n] /= normir;
		}
	}
	for (int x = 0; x < in.rows; ++x) {
		for (int y = 0; y < in.cols; ++y) {
			Vec3b tmp(0,0,0);
			for (int i = -radius; i <= radius; i++) {
				for (int j = -radius; j <= radius; j++) {
					int idx = (i + radius) * size + j + radius;
					tmp[0] += result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[0]*vec[idx];
					tmp[1] += result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[1]*vec[idx];
					tmp[2] += result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[2]*vec[idx];
				}
			}
			result.at<Vec3b>(x, y) = Vec3b(check(tmp[0], 255, 0), check(tmp[1], 255, 0), check(tmp[2], 255, 0));
		}
	}
	return result;
}

vector<pair<int, int>> pos;
void posRand(int n,int m) {
	for (int i = 0; i < (0.05 * n * m); ++i) {
		int px = rand() % n;
		int py = rand() % m;
		pos.push_back(make_pair(px, py));
	}
}
double norm(int a,int b) {
	double x = 0;
	for (int i = 0; i < 12; ++i)  
		x += (rand() % 101) * 0.01;
	return x - (b+a)/2;
}
vector<vector<double>> g;
void calc(int n, int a,int b) {
	for (int i = 0; i < n; ++i)  
		g[pos[i].first][pos[i].second] = a + (b - a) * norm(a,b);
}

int main() {
	String im_name("../data/car.jpg");
	Mat img1 = imread(im_name);
	Mat outImg, med, gauss;
	imshow("IMG1", img1);
	waitKey();
	
	//constant noise
	Mat res=img1;
	int n = img1.rows;
	int m = img1.cols;
	g.resize(n, vector<double>(m, 0));
	long double T1 = clock();
	posRand(n, m);
	calc(pos.size(),0,1);
	for (int i = 0; i < pos.size(); ++i) {
		int x = pos[i].first;
		int y = pos[i].second;
		res.at<Vec3b>(x, y) *= g[x][y];
	}
	long double T2 = clock();
	T2 -= T1;
	cout << "noise time:  " << fixed << setprecision(20) << T2 / CLOCKS_PER_SEC << '\n';
	imshow("noise", res);
	waitKey();
	
	//opencv median
	long double t1 = clock();
	medianBlur(res, outImg, 5);
	long double t2 = clock();
	t2 -= t1;
	cout << "medianBlur opencv:  " << fixed << setprecision(20) << t2 / CLOCKS_PER_SEC << '\n';
	imshow("opencvMedianBlur", outImg);
	waitKey();
	
	//median
	long double t11 = clock();
	med = median(res, 1);
	long double t22 = clock();
	t22 -= t11;
	cout << "median:  " << fixed << setprecision(20) << t22 / CLOCKS_PER_SEC << '\n';
	imshow("median", med);
	waitKey();
	
	//gaussian
	long double t111 = clock();
	gauss = gaussian(res, 3, 0.02);
	long double t222 = clock();
	t222 -= t111;
	cout << "gaussian:  " << fixed << setprecision(20) << t222 / CLOCKS_PER_SEC << '\n';
	imshow("Gaussian", gauss);
	waitKey();
	return 0;
}
