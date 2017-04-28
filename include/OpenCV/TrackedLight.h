#ifndef TRACKEDLIGHT_H
#define TRACKEDLIGHT_H
#include "opencv2/core/types_c.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Core/Core.h"
namespace OpenCV {

/**
	 * @author Autor: Marek Karas
	 * @brief The TrackedLight Class for representing tracked lights
	 * @date 11.4.2017
	 */
class TrackedLight
{
public:
	TrackedLight();
	~TrackedLight();

	void MapToHemisphere( cv::Point2f fisheye_center, float fisheye_radius );

	cv::Point2f			framePosition;
	osg::Vec4f			hemispherePosition;
	osg::Vec4f			color;
	float				radius;
	bool				active;

	int id;
	bool found;
	//std::vector<cv::Point3f> history;
};
}
#endif
