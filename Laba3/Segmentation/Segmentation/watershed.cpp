#include "header.h"

Mat watershed(Mat im) {
    Mat gray, thresh, dist, fg,bg;
    cvtColor(im, gray, COLOR_BGR2GRAY);
    threshold(gray, thresh, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);
    //imshow("1", thresh);
    //waitKey();

    dilate(thresh, thresh, Mat::ones(3, 3, CV_8U));
    imshow("2", thresh);
    ////waitKey();

    DistanceTransform(thresh, dist, DIST_L2, 5);
    normalize(dist, dist, 0, 1.0, NORM_MINMAX);
    imshow("DistTransform", dist);
    //waitKey();

    double maxVal = 0.0;
    minMaxLoc(dist, 0, &maxVal);
    threshold(dist, fg, 0.7 * maxVal, 255, THRESH_BINARY);
    //imshow("3", fg);
    //waitKey();
    Mat fg_8u;
    fg.convertTo(fg_8u, CV_8U);

    vector<vector<Point> > conts;
    vector<Vec3b> colors;

    findContours(fg_8u, conts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Mat markers = Mat::zeros(dist.size(), CV_32S);
    for (size_t i = 0; i < conts.size(); ++i) {
        drawContours(markers, conts, static_cast<int>(i), Scalar(static_cast<int>(i) + 1), -1);
    }

    circle(markers, Point(5, 5), 3, Scalar(255), -1);
    watershed(im, markers);

    Mat m;
    markers.convertTo(m, CV_8U);
    bitwise_not(m, m);
    //imshow("m", m);
    //waitKey();
    for (size_t i = 0; i < conts.size(); ++i) {
        int b, g, r;
        b = theRNG().uniform(0, 255);
        g = theRNG().uniform(0, 255);
        r = theRNG().uniform(0, 255);
        colors.push_back(Vec3b(b, g, r));
    }
    Mat ans = Mat::zeros(markers.size(), CV_8UC3);

    for (int i = 0; i < markers.rows; ++i) {
        for (int j = 0; j < markers.cols; ++j) {
            int q = markers.at<int>(i, j);
            if (q > 0 && q <= static_cast<int>(conts.size())) {
                ans.at<Vec3b>(i, j) = colors[q - 1];
            }
        }
    }
    return ans;
}