#include "Leap/HandModule/HandTrackerAdapter.h"

#include <vector>

Leap::HandTrackerAdapter::HandTrackerAdapter()
{
    this->tracker = new OpenCV::HandTracker();
}

Leap::HandTrackerAdapter::~HandTrackerAdapter()
{

}

void Leap::HandTrackerAdapter::trackHands( unsigned char* buffer, float depth)
{
    cv::Mat matrix(240, 640, CV_8UC1, &buffer[0]);
    this->tracker->findHand(matrix, depth);
}
