#ifndef KINECTRECOGNITION_H
#define KINECTRECOGNITION_H

#include "OpenNI.h"
#include "QImage"

#include <opencv2/opencv.hpp>

namespace Kinect {

/**
 * @author Matej Marconak
 * @brief Kinect class for start recognition and conversion of VideoFrame
 * @brief //TODO divide class for open and for converted cv:Mat - Strategy
 */
class KinectRecognition
{

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
	cv::Mat depthImageCvMat( const openni::VideoFrameRef& colorFrame );

	/**
	 * @brief convert videoframe getting from classing frame
	 * @param colorFrame color videoframe
	 * @return converted RGB cv::Mat
	 */
	cv::Mat colorImageCvMat( const openni::VideoFrameRef& colorFrame );

	/**
	 * @brief convert videoframe getting from classing frame
	 * @param colorFrame color videoframe
	 * @return converterd QImage
	 */
	QImage colorImageQImage( const openni::VideoFrameRef& colorFrame );

	/**
	 * @brief convert depth videoframe to QImage
	 * @param colorFrame depth videoframe
	 * @return coverted colofrmae to QImage
	 */
	QImage deptImageQImage( const openni::VideoFrameRef& colorFrame );

	/**
	 * @brief open status of openni
	 * @return true if openni is open, other false
	 */
	bool isOpenOpenni();

	/**
	 * @brief Kinect device
	 */
	openni::Device device;
private:

	/**
	 * @brief is Kinect conection opened
	 */
	bool isOpen;

	/**
	 * @brief status of OpenNI
	 */
	openni::Status mOpenniStatus;

};
}
#endif // KINECTRECOGNITION_H
