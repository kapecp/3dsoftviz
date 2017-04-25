#ifndef LEAPCAMERASTREAM_H
#define LEAPCAMERASTREAM_H

#include <osg/Image>
#include <osg/Geometry>

#include <opencv2/core/core.hpp>
#include "OpenCV/HandTracker.h"

namespace Leap {

class LeapCameraStream : public osg::Image
{
public:
	LeapCameraStream( );
	~LeapCameraStream();

    int getImageWidth();
    int getImageHeight();
    cv::vector<cv::vector<cv::Point>> contourPointList;
public:
	void updateBackgroundImage( unsigned char* buffer );
    OpenCV::HandTracker *tracker;
};
}
#endif
