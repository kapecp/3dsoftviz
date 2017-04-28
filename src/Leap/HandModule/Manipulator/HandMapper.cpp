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

cv::vector<cv::Point> Leap::HandMapper::getHandPointList(cv::vector<cv::vector<cv::Point>> contourPointList) {
    for (int n = 0; n < contourPointList.size(); n++)
    {
        if (contourPointList[n].size() > 4 && contourPointList[n].size() < 7) {
            return contourPointList[n];
        }
    }

    return cv::vector<cv::Point>();
}

float Leap::HandMapper::calculateAveragePalmFingerDistance(cv::vector<cv::Point> handPointList) {
    float averageDst = 0;

    if (handPointList.size() == 0) {
        return averageDst;
    }

    cv::Point palmCenter = handPointList[0];

    for (int n = 1; n < handPointList.size(); n++)
    {
        cv::Point fingerTip = handPointList[n];
        averageDst += cv::norm(palmCenter-fingerTip);
    }

    return averageDst / (handPointList.size() - 1);
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
        cv::vector<cv::vector<cv::Point>> contourPointList =
                this->coreGraph->getCameraStream()->contourPointList;
        float averageDst = this->calculateAveragePalmFingerDistance(this->getHandPointList(contourPointList));

//        LOG( INFO ) << "Point count: " + std::to_string(pointList.size());
        if (averageDst != 0) {

            LOG( INFO ) << "Average distance: " + std::to_string(averageDst);
        }
        //TODO edit vector based on average distance
    }

    return vector;
}

