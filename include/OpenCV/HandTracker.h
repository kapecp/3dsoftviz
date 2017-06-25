#ifndef HANDTRACKER_H
#define HANDTRACKER_H

#include "opencv2/core/types_c.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace OpenCV {

class HandTracker
{

public:

	HandTracker();


	~HandTracker();
	/**
	 * @brief dist calculate distance between two points
	 * @param x
	 * @param y
	 * @return
	 */
	double dist( cv::Point x,cv::Point y );
	/**
	 * @brief circleFromPoints - This function returns the radius and the center of the circle given 3 points
	 * @param p1
	 * @param p2
	 * @param p3
	 * @return
	 */
	std::pair<cv::Point,double> circleFromPoints( cv::Point p1, cv::Point p2, cv::Point p3 );
	/**
	 * @brief getParameterValues - calculates new distance from camera, based on old one track by leap sensor
	 * @param threshold
	 * @param areaSize
	 * @param brightness
	 * @param depth
	 */
	void getParameterValues( int* threshold, int* areaSize, float brightness, float depth );
	/**
	 * @brief produceBinaries - return greyscale image from RGB image
	 * @param m - mat of image
	 * @return
	 */
	cv::Mat produceBinaries( cv::Mat m );
	/**
	 * @brief findHand - finds center of the palm and radsiu of the palm
	 * @param mask
	 * @return
	 */
	cv::vector<std::pair<cv::Point,double>> findHand( cv::Mat mask );



};
}

#endif // HANDTRACKER_H
