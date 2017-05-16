#ifndef TRACKERBALL_H
#define TRACKERBALL_H
#include "opencv2/core/types_c.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Core/Core.h"
namespace OpenCV {

/**
	 * @author Autor: Marek Rostar
	 * @brief The TrackerBall class for representing tracked balls
	 * @date 11.4.2017
	 */
class TrackerBall
{
public:
	TrackerBall();
	~TrackerBall();
	void findBall( cv::Mat image );
	std::vector<cv::Point3f> history;
	cv::Point2f cent;
	float radius;
	float lastInlierPerc;
	int id;
	int numOfFramesnfound;
	bool found;

};
}
#endif
