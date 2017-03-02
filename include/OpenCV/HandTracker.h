#ifndef HANDTRACKER_H
#define HANDTRACKER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Core/Core.h"

namespace OpenCV {

class HandTracker
{

public:

    HandTracker();

    ~HandTracker();

    cv::Mat findHand( cv::Mat mask );
};
}

#endif // HANDTRACKER_H
