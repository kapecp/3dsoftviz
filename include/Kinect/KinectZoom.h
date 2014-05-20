#ifndef KINECTZOOM_H
#define KINECTZOOM_H

#include "OpenNI.h"
#include "NiTE.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Kinect{

class KinectZoom
{

public:

	KinectZoom();

	~KinectZoom();

	void zoom(cv::Mat handImage, openni::VideoStream  *m_depth, float x, float y, float z);
	void DetectContour(cv::Mat img);
};
}

#endif // KINECTZOOM_H
