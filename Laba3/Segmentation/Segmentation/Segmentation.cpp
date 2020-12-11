#include "header.h"

int main() {
	String im_name("../data/15.jpg");
	Mat img1 = imread(im_name);
	/*imshow("IMG1", img1);
	waitKey();*/

	/********************Canny*************************/
	Mat im = img1;
	Mat gray = Mat(toGrayScale(im));
	//imshow("Gray", gray);
	//waitKey();
	Mat gauss = Mat(Gauss(gray,3, 0.02));
	//imshow("Gauss", gauss);
	//waitKey();
	Mat sobel = Mat(Sobel(gauss));
	//imshow("Sobel", sobel);
	//waitKey();
	Mat non = Mat(nonMax(sobel));
	imshow("Non", non);
	//waitKey();
	Mat res = Mat(threshold(non,20,40));
	imshow("Canny", res);
	waitKey();

	Mat imm = img1;
	cvtColor(imm, imm, COLOR_BGR2GRAY);
	Canny(imm, imm, 20, 40);
	imshow("CannyoPENCV", imm);
	waitKey();
	/********************Distance*************************/
	String im_n("../data/coins.jpg");
	Mat im0 = imread(im_n);
	cvtColor(im0, im0, COLOR_BGR2GRAY);
	threshold(im0, im0, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);
	dilate(im0, im0, Mat::ones(3, 3, CV_8U));
	Mat dist;
	DistanceTransform(im0, dist, DIST_L2, 5);
	normalize(dist, dist, 0, 1.0, NORM_MINMAX);
	imshow("dist", dist);
	waitKey();

	/********************Watershed*************************/
	String im_name2("../data/3.jpg");
	Mat img2 = imread(im_name2);
	Mat water = watershed(img2);
	imshow("watershed", water);
	waitKey();




	return 0;
}