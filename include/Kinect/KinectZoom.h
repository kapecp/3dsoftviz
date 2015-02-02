#ifndef KINECTZOOM_H
#define KINECTZOOM_H

#include "OpenNI.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Core/Core.h"

namespace Kinect{
/**
 * @brief The KinectZoom class
 * @author autor: Marek Jakab
 */
class KinectZoom
{

public:

	/**
	 * @brief KinectZoom constructor to initialize values
	 */
	KinectZoom();

	~KinectZoom();

	/**
	 * @brief zoom start scroll mouse based on hand depth
	 * @param handImage image of hand
	 * @param m_depth depth image of hand
	 * @param x world coordinate X of hand
	 * @param y world coordinate Y of hand
	 * @param z world coordinate Z of hand
	 */
	void zoom(cv::Mat handImage, openni::VideoStream  *m_depth, float x, float y, float z);
	/**
	 * @brief DetectContour find contours of segmented hand image
	 * @param img image to find contour on
	 * @return number of fingers found (if the hand is open)
	 */
	int DetectContour(cv::Mat img);

	/**
	 * @brief previousZ depth of hand from previous frame
	 */
	float previousZ;
	/**
	 * @brief currentZ depth of hand from current frame
	 */
	float currentZ;
	/**
	 * @brief delta
	 */
	float delta;
	/**
	 * @brief zoomThreshold set threshold for hand movement
	 */
	float zoomThreshold;

	/**
	 * @brief viewer QT Viewer to simulate mouse scroll
	 */
	QOSG::ViewerQT *viewer;
};
}

#endif // KINECTZOOM_H
