#ifndef MARKERLESSTRACKER_H
#define MARKERLESSTRACKER_H

#include <opencv2/opencv.hpp>

namespace OpenCV {

class MarkerlessTracker;

/**
	 * @author Autor: Lukas Hagara
	 * @brief The MarkerlessTracker class
	 * @date 24.2.2017
	 */
class MarkerlessTracker
{

public:

	MarkerlessTracker();
	~MarkerlessTracker();

	/**
	 * @author Autor: Lukas Hagara
	 * @brief track white circles on image
	 * @param img to track objects on
	 */
	void track( cv::Mat& img );

};
}

#endif // MARKERLESSTRACKER_H
