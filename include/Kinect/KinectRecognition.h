#ifndef KINECTRECOGNITION_H
#define KINECTRECOGNITION_H

#include "OpenNI.h"
#include "NiTE.h"
#include <opencv2/opencv.hpp>
namespace Kinect{

/**
 * @author Matej Marconak
 *@brief The KinectRecognition class
 */

class KinectRecognition{

public:
	KinectRecognition();
	~KinectRecognition();

	cv::Mat depthImage(nite::HandTrackerFrameRef& handFrame);

	cv::Mat colorImage(openni::VideoFrameRef& normalFrame);
private:

	openni::Device device;
	nite::HandTracker handTracker;
	nite::Status mNiteStatus;
	openni::Status mOpenniStatus;
	openni::VideoStream  color;

};
}
#endif // KINECTRECOGNITION_H
