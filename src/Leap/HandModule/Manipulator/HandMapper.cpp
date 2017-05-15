#include "Leap/HandModule/Manipulator/HandMapper.h"

#include "Viewer/CoreGraph.h"

#include <vector>
#include <easylogging++.h>

#ifdef OPENCV_FOUND
#include "OpenCV/CameraStream.h"
#endif

Leap::HandMapper::HandMapper( Vwr::CoreGraph* coreGraph )
	:coreGraph( coreGraph ), cameraOffset( 0 )
{
}

Leap::HandMapper::~HandMapper()
{

}

cv::vector<cv::Point> Leap::HandMapper::getHandPointList( cv::vector<cv::vector<cv::Point>> contourPointList )
{
	for ( int n = 0; n < contourPointList.size(); n++ ) {
		if ( contourPointList[n].size() > 3 && contourPointList[n].size() < 7 ) {
			return contourPointList[n];
		}
	}

	return cv::vector<cv::Point>();
}

float Leap::HandMapper::calculateAveragePalmFingerDistance( cv::vector<cv::Point> handPointList,
		int imageWidth, int imageHeight )
{
	float averageDst = 0;

	if ( handPointList.size() == 0 ) {
		return averageDst;
	}

	cv::Point2f palmCenter;
	palmCenter = cv::Point2f( static_cast<float>( handPointList[0].x ),static_cast<float>( handPointList[0].y ) );
	palmCenter.x /= imageWidth;
	palmCenter.y /= imageHeight;

	for ( int n = 1; n < handPointList.size(); n++ ) {
		cv::Point2f fingerTip;
		fingerTip = cv::Point2f( static_cast<float>( handPointList[n].x ),static_cast<float>( handPointList[n].y ) );
		fingerTip.x /= imageWidth;
		fingerTip.y /= imageHeight;
		averageDst += cv::norm( palmCenter-fingerTip );
	}

	return averageDst / ( handPointList.size() - 1 );
}

Leap::Vector Leap::HandMapper::recalculateDepthNode( Leap::Vector vector, float diff )
{

	if ( this->coreGraph->isLeapStreamActive() ) {
		if ( diff > 0 ) {
			vector.y = vector.y + diff*2.3;

			vector.y = vector.y + vector.x*0.4;
		}
		else {
			vector.y = vector.y + diff*2.0;
		}
	}
	else if ( this->coreGraph->isCameraStreamActive() ) {
		this->coreGraph->getCameraStream()->calibrated = true;

		// ofset to make hands bigger
		vector.y = vector.y - 680.0;
		// compesation of camera and leap offset
		vector.z = vector.z - 50;

		diff -= 100;
		// 400+ ruka
		if ( diff > 0 ) {
			vector.y = vector.y + diff*-0.4;
		} // ruka 0 - 400
		else {
			vector.y = vector.y + diff*-0.2;
		}


//        float averageDst = 0;
//        if (!this->coreGraph->getCameraStream()->calibrated) {
//            cv::vector<cv::vector<cv::Point>> contourPointList =
//                    this->coreGraph->getCameraStream()->contourPointList;
//            averageDst = this->calculateAveragePalmFingerDistance(
//                    this->getHandPointList(contourPointList),
//                    this->coreGraph->getCameraStream()->getImageWidth(),
//                    this->coreGraph->getCameraStream()->getImageHeight());
//        }

////        LOG( INFO ) << "Point count: " + std::to_string(pointList.size());
//        if (this->cameraOffset !=0 || averageDst != 0) {
//            this->coreGraph->getCameraStream()->calibrated = true;
////            LOG(INFO) << "calibrated";
//            //this->cameraOffset = averageDst * vector.y;
//            if (averageDst != 0){
//                this->cameraOffset = averageDst;
//            }

//            diff -= 100;
//            // 400+ ruka
////            if (diff > 0){
////                vector.y = vector.y + diff*this->cameraOffset*1.3;
////            } // ruka 0 - 400
////            else{
////                vector.y = vector.y + diff*this->cameraOffset*1.2;
////            }
//            LOG( INFO ) << "Hand scene position y PRED: " + std::to_string(vector.y);

//            LOG( INFO ) << "Hand scene position y PO: " + std::to_string(vector.y);
//        }
//        //TODO edit vector based on average distance
	}

	return vector;
}

