#include "header.h"
Mat angles;
template <class T>
T check(T v, int max, int min) {
    if (v > max) return max;
    else if (v < min) return min;
    else return v;
}

Mat toGrayScale(Mat& im) {
	im.forEach<Vec3b>([](Vec3b& p, const int* pos) {
		int m = p[0] + p[1] + p[2];
		m /= 3;
		p[0] = m;
		p[1] = m;
		p[2] = m;

		});
	return im;
}

Mat Sobel(Mat im) {
    vector<vector<double>> x(3);
    vector<vector<double>> y(3);
    x[0] = { -1.0, 0, 1.0 };
    x[1] = { -2.0, 0, 2.0 };
    x[2] = { -1.0, 0, 1.0 };
    y[0] = { 1.0, 2.0, 1.0 };
    y[1] = { 0, 0, 0 };
    y[2] = { -1.0, -2.0, -1.0 };


    Mat result = Mat(im.rows - 2, im.cols - 2, CV_8UC3);
    angles = Mat(im.rows - 2 , im.cols - 2 , CV_32FC3);

    for (int i = 1; i < im.rows - 1; ++i) {
        for (int j = 1; j < im.cols - 1; ++j) {
            double sumxR = 0;
            double sumxG = 0;
            double sumxB = 0;
            double sumyR = 0;
            double sumyG = 0;
            double sumyB = 0;

            for (int ii = 0; ii < x.size(); ++ii) {
                for (int jj = 0; jj < x.size(); ++jj) {
                    sumxB += x[ii][jj] * static_cast<double>((im.at<Vec3b>(i + ii - 1, j + jj - 1))[0]);
                    sumyB += y[ii][jj] * static_cast<double>((im.at<Vec3b>(i + ii - 1, j + jj - 1))[0]);
                    sumxG += x[ii][jj] * static_cast<double>((im.at<Vec3b>(i + ii - 1, j + jj - 1))[1]);
                    sumyG += y[ii][jj] * static_cast<double>((im.at<Vec3b>(i + ii - 1, j + jj - 1))[1]);
                    sumxR += x[ii][jj] * static_cast<double>((im.at<Vec3b>(i + ii - 1, j + jj - 1))[2]);
                    sumyR += y[ii][jj] * static_cast<double>((im.at<Vec3b>(i + ii - 1, j + jj - 1))[2]);
                }
            }
            Vec3b sq2(0, 0, 0);
            sq2[0] = sqrt(sumxB * sumxB + sumyB * sumyB);
            sq2[1] = sqrt(sumxG * sumxG + sumyG * sumyG);
            sq2[2] = sqrt(sumxR * sumxR + sumyR * sumyR);

            if (sq2[0] > 255 || sq2[1] > 255 || sq2[2] > 255) {
                sq2 = Vec3b(255, 255, 255);
            }
            result.at<Vec3b>(i - 1, j - 1) = sq2;

            if (sumxR == 0 || sumxG == 0 || sumxB == 0) {
                angles.at<Vec3f>(i - 1, j - 1) = Vec3f(90, 90, 90);
            }
            else {
                angles.at<Vec3f>(i - 1, j - 1) = Vec3f(atan(sumyB / sumxB), atan(sumyG / sumxG), atan(sumyR / sumxR));
            }
        }
    }
    return result;
}

Mat nonMax(Mat im) {
    Mat result = Mat(im.rows - 2, im.cols - 2, CV_8UC3);
    for (int i = 1; i < im.rows - 1; i++) {
        for (int j = 1; j < im.cols - 1; j++) {
            Vec3f t(0,0,0);
            t = angles.at<Vec3f>(i, j);
            result.at<Vec3b>(i - 1, j - 1) = im.at<Vec3b>(i-1, j-1);
            //горизонтальное
            if ((((-22.5 < t[0]) && (t[0] <= 22.5)) || ((157.5 < t[0]) && (t[0] <= -157.5))) &&
                (((-22.5 < t[1]) && (t[1] <= 22.5)) || ((157.5 < t[1]) && (t[1] <= -157.5))) &&
                (((-22.5 < t[2]) && (t[2] <= 22.5)) || ((157.5 < t[2]) && (t[2] <= -157.5))) ){
                if (((im.at<Vec3b>(i, j)[0] < im.at<Vec3b>(i, j + 1)[0]) || (im.at<Vec3b>(i, j)[0] < im.at<Vec3b>(i, j - 1)[0]))&&
                    ((im.at<Vec3b>(i, j)[1] < im.at<Vec3b>(i, j + 1)[1]) || (im.at<Vec3b>(i, j)[1] < im.at<Vec3b>(i, j - 1)[1]))&&
                    ((im.at<Vec3b>(i, j)[2] < im.at<Vec3b>(i, j + 1)[2]) || (im.at<Vec3b>(i, j)[2] < im.at<Vec3b>(i, j - 1)[2])))
                    result.at<Vec3b>(i - 1, j - 1) = Vec3b(0, 0, 0);
            }
           
            //вертикальное
            if ((((-112.5 < t[0]) && (t[0] <= -67.5)) || ((67.5 < t[0]) && (t[0] <= 112.5)))&&
                (((-112.5 < t[1]) && (t[1] <= -67.5)) || ((67.5 < t[1]) && (t[1] <= 112.5))) &&
                (((-112.5 < t[2]) && (t[2] <= -67.5)) || ((67.5 < t[2]) && (t[2] <= 112.5))) ){
                if (((im.at<Vec3b>(i, j)[0] < im.at<Vec3b>(i + 1, j)[0]) || (im.at<Vec3b>(i, j)[0] < im.at<Vec3b>(i - 1, j)[0]))&&
                    ((im.at<Vec3b>(i, j)[1] < im.at<Vec3b>(i + 1, j)[1]) || (im.at<Vec3b>(i, j)[1] < im.at<Vec3b>(i - 1, j)[1]))&&
                    ((im.at<Vec3b>(i, j)[2] < im.at<Vec3b>(i + 1, j)[2]) || (im.at<Vec3b>(i, j)[2] < im.at<Vec3b>(i - 1, j)[2])))
                    result.at<Vec3b>(i - 1, j - 1) = Vec3b(0, 0, 0);
            }

            //-45
            if ((((-67.5 < t[0]) && (t[0] <= -22.5)) || ((112.5 < t[0]) && (t[0] <= 157.5)))&&
                (((-67.5 < t[1]) && (t[1] <= -22.5)) || ((112.5 < t[1]) && (t[1] <= 157.5))) &&
                (((-67.5 < t[2]) && (t[2] <= -22.5)) || ((112.5 < t[2]) && (t[2] <= 157.5))) ){
                if (((im.at<Vec3b>(i, j)[0] < im.at<Vec3b>(i - 1, j + 1)[0]) || (im.at<Vec3b>(i, j)[0] < im.at<Vec3b>(i + 1, j - 1)[0]))&&
                    ((im.at<Vec3b>(i, j)[1] < im.at<Vec3b>(i - 1, j + 1)[1]) || (im.at<Vec3b>(i, j)[1] < im.at<Vec3b>(i + 1, j - 1)[1]))&&
                    ((im.at<Vec3b>(i, j)[2] < im.at<Vec3b>(i - 1, j + 1)[2]) || (im.at<Vec3b>(i, j)[2] < im.at<Vec3b>(i + 1, j - 1)[2])))
                    result.at<Vec3b>(i - 1, j - 1)= Vec3b(0, 0, 0);
            }
            
            //+45
            if ((((-157.5 < t[0]) && (t[0] <= -112.5)) || ((22.5 < t[0]) && (t[0] <= 67.5)))&&
                (((-157.5 < t[1]) && (t[1] <= -112.5)) || ((22.5 < t[1]) && (t[1] <= 67.5))) &&
                (((-157.5 < t[2]) && (t[2] <= -112.5)) || ((22.5 < t[2]) && (t[2] <= 67.5))) ){
                if (((im.at<Vec3b>(i, j)[0] < im.at<Vec3b>(i + 1, j + 1)[0]) || (im.at<Vec3b>(i, j)[0] < im.at<Vec3b>(i - 1, j - 1)[0]))&&
                    ((im.at<Vec3b>(i, j)[1] < im.at<Vec3b>(i + 1, j + 1)[1]) || (im.at<Vec3b>(i, j)[1] < im.at<Vec3b>(i - 1, j - 1)[1]))&&
                    ((im.at<Vec3b>(i, j)[2] < im.at<Vec3b>(i + 1, j + 1)[2]) || (im.at<Vec3b>(i, j)[2] < im.at<Vec3b>(i - 1, j - 1)[2])))
                    result.at<Vec3b>(i - 1, j - 1) = Vec3b(0, 0, 0);
            }
            
        }
    }
    return result;

}

Mat threshold(Mat im, int a, int b) {
    if (a > 255) a = 255;
    if (b > 255) b = 255;
    Mat edge = Mat(im.rows, im.cols, CV_8UC3);

    for (int i = 0; i < im.rows; ++i) {
        for (int j = 0; j < im.cols; ++j) {
            edge.at<Vec3b>(i, j) = im.at<Vec3b>(i, j);
            if (edge.at<Vec3b>(i, j)[0] > b && edge.at<Vec3b>(i, j)[1]>b && edge.at<Vec3b>(i, j)[2]>b) {
                edge.at<Vec3b>(i, j) = Vec3b(255,255,255);
            }
            else if (edge.at<Vec3b>(i, j)[0] < a && edge.at<Vec3b>(i, j)[1]< a && edge.at<Vec3b>(i, j)[2]< a) {
                edge.at<Vec3b>(i, j) = Vec3b(0,0,0);
            }
            else {
                int H = 0, lolo = 0;
                for (int x = i - 1; x <= i + 1; ++x) {
                    for (int y = j - 1; y <= j + 1; ++y) {
                        if (x <= 0 || y <= 0 || edge.rows || y > edge.cols) continue;
                        else {
                            if (edge.at<Vec3b>(x, y)[0] > b && edge.at<Vec3b>(x, y)[1]>b && edge.at<Vec3b>(x, y)[2]>b) {
                                edge.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
                                H = 1;
                                break;
                            }
                            else if (edge.at<Vec3b>(x, y)[0] <= b && edge.at<Vec3b>(x, y)[0] >= a &&
                                        edge.at<Vec3b>(x, y)[1] <= b && edge.at<Vec3b>(x, y)[1] >= a &&
                                        edge.at<Vec3b>(x, y)[2] <= b && edge.at<Vec3b>(x, y)[2] >= a) {
                                lolo = 1;
                            }
                        }
                    }
                    if (H) break;
                }
                if (!H && lolo) {
                    for (int x = i - 2; x <= i + 2; ++x) {
                        for (int y = j - 2; y <= j + 2; ++y) {
                            if (x < 0 || y < 0 || x > edge.rows || y > edge.cols) continue;
                            else {
                                if (edge.at<Vec3b>(x, y)[0] > b && edge.at<Vec3b>(x, y)[1]>b && edge.at<Vec3b>(x, y)[2]>b) {
                                    edge.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
                                    H = 1;
                                    break;
                                }
                            }
                        }
                        if (H) break;
                    }
                }
                if (!H) {
                    edge.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
                }
            }
        }
    }
    return edge;
}

Mat Gauss(Mat& in, int radius,float sigma) {
    Mat result(in);
    int size = 2 * radius + 1;//размер ядра
    float normir = 0;//коэффициент нормировки ядра
    vector<float>vec(size * size);//ядро фильтра
    for (int i = -radius; i <= radius; i++) {//ядро линейного фильтра
        for (int j = -radius; j <= radius; j++) {
            int idx = (i + radius) * size + (j + radius);
            vec[idx] = exp(-(i * i + j * j) / (2 * sigma * sigma));
            vec[idx] /= (2 * 3.14 * sigma * sigma);
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
            Vec3b tmp(0, 0, 0);
            for (int i = -radius; i <= radius; i++) {
                for (int j = -radius; j <= radius; j++) {
                    int idx = (i + radius) * size + j + radius;
                    tmp[0] += result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[0] * vec[idx];
                    tmp[1] += result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[1] * vec[idx];
                    tmp[2] += result.at<Vec3b>(check<int>(x + i, in.rows - 1, 0), check<int>(y + j, in.cols - 1, 0))[2] * vec[idx];
                }
            }
            result.at<Vec3b>(x, y) = Vec3b(check(tmp[0], 255, 0), check(tmp[1], 255, 0), check(tmp[2], 255, 0));
        }
    }
    return result;
}