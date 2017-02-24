#ifndef MARKERLESSTRACKER_H
#define MARKERLESSTRACKER_H

#include <opencv2/opencv.hpp>

namespace OpenCV {

class MarkerlessTracker;

class MarkerlessTracker
{

public:

	MarkerlessTracker();
	~MarkerlessTracker();

	void track( cv::Mat& img);

};
}

#endif // MARKERLESSTRACKER_H
