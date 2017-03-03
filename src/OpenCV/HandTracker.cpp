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
    int numFingers = 0;
    cv::Mat drawing = cv::Mat::zeros( mask.size(), CV_8UC3 );
    cv::vector<cv::vector<cv::Point> > contours;
    cv::vector<cv::Vec4i> hierarchy;
    cv::Mat tempMask = mask.clone();

    threshold(tempMask, tempMask, 200/3, 200, cv::THRESH_BINARY);
    GaussianBlur(tempMask, tempMask, cv::Size(3, 1), 2.5, 2.5);
    int erosion_type = cv::MORPH_ELLIPSE;
    int erosion_size = 0;
    cv::Mat element = getStructuringElement(erosion_type, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size));
    erode(tempMask, tempMask, element);
    dilate(tempMask, tempMask, element);

    findContours(tempMask, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
//    findContours( tempMask,contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point() );

    LOG (INFO) << std::to_string(contours.size());

//    random color for each contour
      cv::RNG rng(12345);
      /// Draw contours + hull results
      for( int i = 0; i< contours.size(); i++ )
         {
//           cv::Scalar color = cv::Scalar( 255,255,255);
           cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
           drawContours( mask, contours, i, color, 1, 8, hierarchy, 0, cv::Point(0,0) );
         }

    return mask;
}
