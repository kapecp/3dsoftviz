#include "OpenCV/HandTracker.h"
#include <easylogging++.h>
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
    cv::Mat tempMask = mask.clone();

    threshold(tempMask, tempMask, 200/3, 200, cv::THRESH_BINARY);
    Canny(tempMask,tempMask,200/3,200,3);

    findContours( tempMask,contours, hierarchy, CV_RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point() );

    cv::Scalar color = cv::Scalar( 255,255,255);
    // Draw contours + hull results
    for( int i = 0; i< contours.size(); i++ )
     {
       drawContours( mask, contours, i, color, 1, 8, hierarchy, 0, cv::Point(0,0) );
     }
    return mask;
}
