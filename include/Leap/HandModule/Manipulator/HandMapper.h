#ifndef HANDMAPPER_H
#define HANDMAPPER_H

#include "Leap.h"
#include <opencv2/core/core.hpp>

namespace Vwr {
class CoreGraph;
}

namespace Leap {

class HandMapper
{

public:

	HandMapper( Vwr::CoreGraph* coreGraph );

	~HandMapper();

<<<<<<< HEAD
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
=======
	Leap::Vector recalculateDepthNode( Leap::Vector vector, float diff );



private:
	float calculateAveragePalmFingerDistance( cv::vector<cv::Point> pointList,
			int imageWidth, int imageHeight );
	cv::vector<cv::Point> getHandPointList( cv::vector<cv::vector<cv::Point>> contourPointList );
	Vwr::CoreGraph* coreGraph;
	float cameraOffset;
>>>>>>> 42681eec6f5d3863a6af7f978295a617be8b2ef4
};
}

#endif // HANDMAPPER_H
