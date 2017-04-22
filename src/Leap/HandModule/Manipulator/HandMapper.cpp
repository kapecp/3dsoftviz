#include "Leap/HandModule/Manipulator/HandMapper.h"

#include <vector>
#include <easylogging++.h>

#ifdef OPENCV_FOUND
#include "OpenCV/CameraStream.h"
#endif

Leap::HandMapper::HandMapper(Vwr::CoreGraph* coreGraph)
    :coreGraph(coreGraph)
{
}

Leap::HandMapper::~HandMapper()
{

}

float Leap::HandMapper::calculateAveragePalmFingerDistance(cv::vector<cv::Point> pointList) {
    float averageDst = 0;
    if (pointList.size() > 1) {
        cv::Point palmCenter = pointList[0];

        for (int n = 1; n < pointList.size(); n++)
        {
            cv::Point fingerTip = pointList[n];
            averageDst += cv::norm(palmCenter-fingerTip);
        }

        return averageDst / (pointList.size() - 1);
    }

    return averageDst;
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
        cv::vector<cv::Point> pointList = this->coreGraph->getCameraStream()->handPointList;
        float averageDst = this->calculateAveragePalmFingerDistance(pointList);
        LOG( INFO ) << "Average distance: " + std::to_string(averageDst);

        //TODO edit vector based on average distance
    }

    return vector;
}

