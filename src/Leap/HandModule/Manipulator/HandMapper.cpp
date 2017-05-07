#include "Leap/HandModule/Manipulator/HandMapper.h"

#include <vector>
#include <easylogging++.h>

#ifdef OPENCV_FOUND
#include "OpenCV/CameraStream.h"
#endif

Leap::HandMapper::HandMapper(Vwr::CoreGraph* coreGraph)
    :coreGraph(coreGraph), cameraOffset(cv::Point(0,0)), calibrationCounter(0),
      calibrationLastPoint(cv::Point(0,0))
{

}

Leap::HandMapper::~HandMapper()
{

}

cv::vector<cv::Point> Leap::HandMapper::getHandPointList(cv::vector<cv::vector<cv::Point>> contourPointList) {
    for (int n = 0; n < contourPointList.size(); n++)
    {
        if (contourPointList[n].size() > 3 && contourPointList[n].size() < 7) {
            return contourPointList[n];
        }
    }

    return cv::vector<cv::Point>();
}

float Leap::HandMapper::calculateAveragePalmFingerDistance(cv::vector<cv::Point> handPointList,
                                                           int imageWidth, int imageHeight) {
    float averageDst = 0;

    if (handPointList.size() == 0) {
        return averageDst;
    }

	cv::Point2f palmCenter;
	palmCenter = cv::Point2f(static_cast<float>(handPointList[0].x),static_cast<float>(handPointList[0].y));
	palmCenter.x /= imageWidth;
    palmCenter.y /= imageHeight;

    for (int n = 1; n < handPointList.size(); n++)
    {
		cv::Point2f fingerTip;
		fingerTip = cv::Point2f(static_cast<float>(handPointList[n].x),static_cast<float>(handPointList[n].y));
		fingerTip.x /= imageWidth;
        fingerTip.y /= imageHeight;
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

        // ofset to make hands bigger
        vector.y = vector.y - 680.0;
        // compesation of camera and leap offset
        vector.z = vector.z ;

        diff -= 100;
                   // 400+ ruka
        if (diff > 0){
            vector.y = vector.y + diff*-0.4;
        } // ruka 0 - 400
        else{
            vector.y = vector.y + diff*-0.2;
        }



        cv::vector<std::pair<cv::Point,double>> palmAndRadiusList =
                           this->coreGraph->getCameraStream()->palmAndRadiusList;


  if(!this->coreGraph->getCameraStream()->calibrated){
        if(std::abs(this->nodeScreenCoords.x() - this->calibrationLastPoint.x) < 10 &&
           std::abs(this->nodeScreenCoords.y() - this->calibrationLastPoint.y) < 10){
            this->calibrationCounter += 1;
        }else{
            this->calibrationCounter = 0;
        }
        this->calibrationLastPoint.x = this->nodeScreenCoords.x();
        this->calibrationLastPoint.y = this->nodeScreenCoords.y();
//        LOG (INFO) << std::to_string(this->calibrationCounter);
    }

    if(this->calibrationCounter > 100000){
        LOG (INFO) << std::to_string(this->calibrationCounter);
    }


        // if is palm found and hand is still in leap stream and palm radius is higher than threshold
        if(palmAndRadiusList.size() > 0 && this->calibrationCounter > 50000 && palmAndRadiusList[0].second > 40){
            if(!this->coreGraph->getCameraStream()->calibrated){
//                LOG (INFO) << "this->nodeScreenCoords 0: " + std::to_string(palmPointList[0].x) + " 1: " + std::to_string(palmPointList[0].y);
                this->cameraOffset = cv::Point(int(this->nodeScreenCoords.x()) - palmAndRadiusList[0].first.x, int(this->nodeScreenCoords.y()) - palmAndRadiusList[0].first.y  );
//                LOG (INFO) << "radius: " + std::to_string(palmAndRadiusList[0].second);
            }
            this->coreGraph->getCameraStream()->calibrated = true;
//            LOG (INFO) << "this->nodeScreenCoords 0: " + std::to_string(palmPointList[0].x) + " 1: " + std::to_string(palmPointList[0].y);
        }

        if( this->coreGraph->getCameraStream()->calibrated){
            LOG (INFO) << "x: " + std::to_string(this->cameraOffset.x) +  " y: " + std::to_string(this->cameraOffset.y);
//            float depthOffset = this->cameraOffset.x >= 0 ? vector.y/8 : -vector.y/8 ;
            float depthOffset = vector.y/400 * this->cameraOffset.x/3;
            // TODO depth offset spravit zavysli od this->cameraOffset.x

            vector.x += this->cameraOffset.x * 1.2  + depthOffset;
            vector.z += vector.y/20 ;
        }

    }

    return vector;
}

void Leap::HandMapper::setNodeScreenCoords(osg::Vec3 nodeScreenCoords){
    this->nodeScreenCoords = nodeScreenCoords;
}
