#ifndef KINECTRECOGNITION_H
#define KINECTRECOGNITION_H

#include "OpenNI.h"
#include "QImage"

#include <opencv2/opencv.hpp>

namespace Kinect{

/**
 * @author Matej Marconak
 *@brief Kinect class for start recognition and conversion of VideoFrame
 */

class KinectRecognition{

public:

	KinectRecognition();

	~KinectRecognition();

	/**
	 * @brief inicialize Openni and open Kinnect Device
	 */
	void inicialize();

	/**
	 * @brief close connection with Openni
	 */
	void closeOpenni();

	/**
	 * @brief convert depth videoframe to cv::Mat
	 * @param colorFrame depth videoframe
	 * @return converted gray cv::Mat
	 */
	cv::Mat depthImageCvMat(openni::VideoFrameRef &colorFrame);

	/**
	 * @brief convert videoframe getting from classing frame
	 * @param colorFrame color videoframe
	 * @return converted RGB cv::Mat
	 */
	cv::Mat colorImageCvMat(openni::VideoFrameRef& colorFrame);

	/**
	 * @brief convert videoframe getting from classing frame
	 * @param colorFrame color videoframe
	 * @return
	 */
	QImage colorImageQImage(openni::VideoFrameRef& colorFrame);

	/**
	 * @brief convert depth videoframe to QImage
	 * @param colorFrame depth videoframe
	 * @return
	 */
	QImage deptImageQImage(openni::VideoFrameRef& colorFrame);

	/**
	 * @brief open status of openni
	 * @return true if openni is open, other false
	 */
	bool isOpenOpenni();

	openni::Device device;
private:

	bool isOpen;
	openni::Status mOpenniStatus;

};
}
#endif // KINECTRECOGNITION_H
