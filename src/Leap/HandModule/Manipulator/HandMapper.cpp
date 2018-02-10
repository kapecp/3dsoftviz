#include "Leap/HandModule/Manipulator/HandMapper.h"

#include "Viewer/CoreGraph.h"

#include <vector>
#include <easylogging++.h>

#ifdef OPENCV_FOUND
#include "OpenCV/CameraStream.h"
#endif


Softviz::Leap::HandMapper::HandMapper( Vwr::CoreGraph* coreGraph )
	:coreGraph( coreGraph ), cameraOffset( cv::Point( 0,0 ) ), calibrationCounter( 0 ),
	 calibrationLastPoint( cv::Point( 0,0 ) )
{

}

Softviz::Leap::HandMapper::~HandMapper()
{

}

cv::vector<cv::Point> Softviz::Leap::HandMapper::getHandPointList( cv::vector<cv::vector<cv::Point>> contourPointList )
{
	for ( std::size_t n = 0; n < contourPointList.size(); n++ ) {
		if ( contourPointList[n].size() > 3 && contourPointList[n].size() < 7 ) {
			return contourPointList[n];
		}
	}

	return cv::vector<cv::Point>();
}

float Softviz::Leap::HandMapper::calculateAveragePalmFingerDistance( cv::vector<cv::Point> handPointList,
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

	for ( std::size_t n = 1; n < handPointList.size(); n++ ) {
		cv::Point2f fingerTip;
		fingerTip = cv::Point2f( static_cast<float>( handPointList[n].x ),static_cast<float>( handPointList[n].y ) );
		fingerTip.x /= imageWidth;
		fingerTip.y /= imageHeight;
		averageDst += cv::norm( palmCenter-fingerTip );
	}

	return averageDst / ( handPointList.size() - 1 );
}


Leap::Vector Softviz::Leap::HandMapper::recalculateDepthNode( ::Leap::Vector vector, float diff )
{

	if ( this->coreGraph->isLeapStreamActive() ) {
		if ( diff > 0 ) {
			vector.y = vector.y + diff*2.3f;

			vector.y = vector.y + vector.x*0.4f;
		}
		else {
			vector.y = vector.y + diff*2.0f;
		}
	}
	else if ( this->coreGraph->isCameraStreamActive() ) {

		// ofset to make hands bigger
		vector.y = vector.y - 680.0f;

		diff -= 100;
		// hand is further than 400
		if ( diff > 0 ) {
			vector.y = vector.y + diff*-0.4f;
		} // hand closer than 400
		else {
			vector.y = vector.y + diff*-0.2f;
		}
		// retrieve palm position and palm radius
		cv::vector<std::pair<cv::Point,double>> palmAndRadiusList =
				this->coreGraph->getCameraStream()->palmAndRadiusList;

		// check if model of leap hand is not moving for certain time interval
		if ( !this->coreGraph->getCameraStream()->calibrated ) {
			if ( std::abs( this->nodeScreenCoords.x() - this->calibrationLastPoint.x ) < 10 &&
					std::abs( this->nodeScreenCoords.y() - this->calibrationLastPoint.y ) < 10 ) {
				this->calibrationCounter += 1;
			}
			else {
				this->calibrationCounter = 0;
			}
			this->calibrationLastPoint.x = static_cast<int>( this->nodeScreenCoords.x() );
			this->calibrationLastPoint.y = static_cast<int>( this->nodeScreenCoords.y() );
		}

		// keep calibrationCounter from overflow
		if ( this->calibrationCounter > 100000 ) {
			if ( this->calibrationCounter > 1000000 ) {
				this->calibrationCounter = 100000;
			}

		}

		// if is palm found and hand is still in leap stream and palm radius is higher than threshold calibrate
		if ( palmAndRadiusList.size() > 0 && this->calibrationCounter > 50000 && palmAndRadiusList[0].second > 40 ) {
			if ( !this->coreGraph->getCameraStream()->calibrated ) {
				this->cameraOffset = cv::Point( int( this->nodeScreenCoords.x() ) - palmAndRadiusList[0].first.x, int( this->nodeScreenCoords.y() ) - palmAndRadiusList[0].first.y );
			}
			this->coreGraph->getCameraStream()->calibrated = true;
			this->calibrationCounter = 0;
		}

		if ( this->coreGraph->getCameraStream()->calibrated ) {
			float depthOffset = vector.y/400 * this->cameraOffset.x/3;

			// calculate hand position based on offsets
			vector.x += this->cameraOffset.x * 1.2f  + depthOffset;
			vector.z += vector.y/20 ;
		}
	}

	return vector;
}

void Softviz::Leap::HandMapper::setNodeScreenCoords( osg::Vec3 nodeScreenCoords )
{
	this->nodeScreenCoords = nodeScreenCoords;
}
