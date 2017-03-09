#include "Leap/HandModule/HandTrackerAdapter.h"
#include "OpenCV/HandTracker.h"

#include <vector>

Leap::HandTrackerAdapter::HandTrackerAdapter()
{
}

Leap::HandTrackerAdapter::~HandTrackerAdapter()
{

}

cv::Mat Leap::HandTrackerAdapter::convertBuffer( unsigned char* buffer )
{
    cv::Mat matrix(240, 640, CV_8UC1, &buffer[0]);
    OpenCV::HandTracker *tracker = new OpenCV::HandTracker();
    tracker->findHand(matrix);

    return matrix;
}
