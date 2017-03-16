#include "Leap/HandModule/HandTrackerAdapter.h"
#include "OpenCV/HandTracker.h"

#include <vector>

Leap::HandTrackerAdapter::HandTrackerAdapter()
{
}

Leap::HandTrackerAdapter::~HandTrackerAdapter()
{

}

void Leap::HandTrackerAdapter::trackHands( unsigned char* buffer, float depth)
{
    cv::Mat matrix(240, 640, CV_8UC1, &buffer[0]);
    OpenCV::HandTracker *tracker = new OpenCV::HandTracker();
    tracker->findHand(matrix, depth);
}
