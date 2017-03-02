#include "OpenCV/HandTracker.h"

#include <vector>

OpenCV::HandTracker::HandTracker()
{
}

OpenCV::HandTracker::~HandTracker()
{

}

// find contours of segmented hand and count fingers
cv::Mat OpenCV::HandTracker::findHand( cv::Mat mask )
{
    cv::vector<cv::vector<cv::Point> > contours;
    cv::vector<cv::Vec4i> hierarchy;

    findContours( mask, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point() );

    return mask;
}
