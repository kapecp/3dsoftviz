#ifndef HANDMAPPER_H
#define HANDMAPPER_H

#include "Leap.h"
#include <opencv2/core/core.hpp>

#include <osg/Vec3>

namespace Vwr {
class CoreGraph;
}

namespace Leap {

class HandMapper
{

public:

	HandMapper( Vwr::CoreGraph* coreGraph );

	~HandMapper();


	Leap::Vector recalculateDepthNode(Leap::Vector vector, float diff);
	void setNodeScreenCoords(osg::Vec3 nodeScreenCoords);


private:
	float calculateAveragePalmFingerDistance(cv::vector<cv::Point> pointList,
											 int imageWidth, int imageHeight);
	cv::vector<cv::Point> getHandPointList(cv::vector<cv::vector<cv::Point>> contourPointList);
	Vwr::CoreGraph* coreGraph;
	cv::Point cameraOffset;
	osg::Vec3 nodeScreenCoords;
	unsigned long calibrationCounter;
	cv::Point calibrationLastPoint;

};
}

#endif // HANDMAPPER_H
