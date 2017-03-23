#include "OpenCV/LightDetector.h"

#include <opencv2/core/core_c.h>

#include <QDebug>

// constructor loads classifier file with learned faces and set start parameters
OpenCV::LightDetector::LightDetector()
{
    a = 0;
}

OpenCV::LightDetector::~LightDetector()
{
}

cv::Mat LightDetector::CircleMask(cv::Mat src, cv::Point center, int radius) {
    cv::Mat out;
    cv::Mat mask = cv::Mat::zeros(src.size(), CV_8U);
    circle(mask, center, radius, CV_RGB(255,255,255), CV_FILLED);
    src.copyTo(out, mask);
    return out;
}


void LightDetector::Threshold(int, void*) {

    // mask
    src_thre = CircleMask(src_gray, Point(fisheye_x, shot_center_y), shot_radius);

    // threshold
    threshold(src_thre, src_thre, threshold_value, max_BINARY_value, THRESH_BINARY);

    imshow(window_name, src_thre);
    // remove noise
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

    morphologyEx(src_thre, src_thre, MORPH_OPEN, kernel, Point(-1, -1), 1);
    morphologyEx(src_thre, src_thre, MORPH_CLOSE, kernel, Point(-1, -1), 1);

    imshow("morph", src_thre);
    LightEstimation_FindLights(0, 0);
}

