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

namespace OpenCV {
class HandTracker;
}

namespace Leap {

class HandMapper
{

public:

    HandMapper(Vwr::CoreGraph* coreGraph);

    ~HandMapper();

    // uses opencv to find hand in image buffer
    void trackHands( unsigned char* buffer,float depth);
    Leap::Vector recalculateDepthNode(Leap::Vector vector, float diff);

private:
    OpenCV::HandTracker *tracker;
    Vwr::CoreGraph* coreGraph;
};
}

#endif // HANDMAPPER_H
