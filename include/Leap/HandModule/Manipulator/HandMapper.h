#ifndef HANDMAPPER_H
#define HANDMAPPER_H

#include <osg/Group>
#include <osg/ref_ptr>
#include <opencv2/core/core.hpp>
#include "OpenCV/HandTracker.h"
#include "Viewer/CoreGraph.h"

namespace Vwr {
class CoreGraph;
}

namespace Leap {

class HandMapper
{

public:

    HandMapper(Vwr::CoreGraph* coreGraph);

    ~HandMapper();

    Leap::Vector recalculateDepthNode(Leap::Vector vector, float diff);



private:  
    float calculateAveragePalmFingerDistance(cv::vector<cv::Point> pointList);

    Vwr::CoreGraph* coreGraph;   
};
}

#endif // HANDMAPPER_H
