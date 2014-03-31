#ifndef KINECTRECOGNITION_H
#define KINECTRECOGNITION_H

#include "OpenNI.h"
#include "NiTE.h"
#include "QImage"
//temporary
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

	cv::Mat depthImageCvMat(openni::VideoFrameRef &colorFrame);

	cv::Mat colorImageCvMat(openni::VideoFrameRef& colorFrame);

	QImage colorImageQImage(openni::VideoFrameRef& colorFrame);

	QImage deptImageQImage(openni::VideoFrameRef& colorFrame);

	bool isOpenOpenni();

	openni::Device device;
private:

	bool isOpen;
	nite::HandTracker handTracker;
	nite::Status mNiteStatus;
	openni::Status mOpenniStatus;
	//openni::VideoStream color;

};
}
#endif // KINECTRECOGNITION_H
