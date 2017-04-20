#include "Leap/HandModule/Manipulator/HandMapper.h"

#include <vector>
#include <easylogging++.h>

Leap::HandMapper::HandMapper(Vwr::CoreGraph* coreGraph)
    :coreGraph(coreGraph)
{
    this->tracker = new OpenCV::HandTracker();
}

Leap::HandMapper::~HandMapper()
{

}

void Leap::HandMapper::trackHands( unsigned char* buffer, float depth)
{
    cv::Mat matrix(240, 640, CV_8UC1, &buffer[0]);
    this->tracker->findHand(matrix, depth);
}

Leap::Vector Leap::HandMapper::recalculateDepthNode(Leap::Vector vector, float diff){

    if (this->coreGraph->isLeapStreamActive()) {
        if (diff > 0){
            vector.y = vector.y + diff*2.3;

            vector.y = vector.y + vector.x*0.4;
        }else{
            vector.y = vector.y + diff*2.0;
        }
    }
    else if (this->coreGraph->isCameraStreamActive()) {

    }

    return vector;
}
