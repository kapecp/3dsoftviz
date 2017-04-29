#include "OpenCV/LightDetector.h"
#include <osg/Vec4d>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <math.h>
#include <opencv2/features2d/features2d.hpp>

#include <QDebug>

// constructor loads classifier file with learned faces and set start parameters
OpenCV::LightDetector::LightDetector()
{
	mFrameMeanColor = osg::Vec4 ( 0.3, 0.3, 0.3, 1.0 );

	for (int i = 0; i < 8; i++)
		mLights.push_back( TrackedLight() );
}

OpenCV::LightDetector::~LightDetector()
{
}

double easeInQuad ( double t, double b, double c, double d ) {
	t /= d;
	return c*t*t + b;
}

double easeOutQuad ( double t, double b, double c, double d ) {
	t /= d;
	return -c * t*(t-2) + b;
}

void OpenCV::LightDetector::DrawBoundary( cv::Mat src )
{
	int circles = 5;
	for ( int i = 1; i <= circles; ++i ) {
		cv::circle( src, this->mfisheyeCenter, (int) easeOutQuad( i, 0, static_cast< float >( this->mFisheyeRadius ), circles ) , CV_RGB( 255,255,255 ) );
	}
}

void OpenCV::LightDetector::DrawLightContours( cv::Mat src )
{
	cv::RNG rng;
	for ( uint i = 0; i< mContours.size(); i++ ) {
		//printf("contour:%d  size:%.2lf pos: x:%.2lf y:%.2lf, projected: x:%.2lf y:%.2lf z:%.2lf\n", i, contourArea(contours[i]), mc[i].x,mc[i].y, est_lights[i].x, est_lights[i].y, est_lights[i].z);
		cv::Scalar color = cv::Scalar( rng.uniform( 0, 255 ), rng.uniform( 0, 255 ), rng.uniform( 0, 255 ) );
		cv::drawContours( src, mContours, static_cast< int >( i ) , color, 2, 8, cv::noArray(), 0, cv::Point() );
	}
}

void OpenCV::LightDetector::DrawLightCircles( cv::Mat src )
{
	for ( size_t i = 0; i< mLightCount; i++ ) {
		cv::circle(src, mLights[i].positionFrame, (int) mLights[i].radius , CV_RGB( 0, 255, 0 ) );
	}
}

void OpenCV::LightDetector::setFisheyeCenter( cv::Point center )
{
	mfisheyeCenter = cv::Point2f( static_cast<float>( center.x ), static_cast<float>( center.y ) );
}

void OpenCV::LightDetector::setFisheyeRadius( int radius )
{
	mFisheyeRadius = static_cast< float >( radius );
}

void OpenCV::LightDetector::setKernelOpen( int size ) {
	mKernelOpen = cv::getStructuringElement( cv::MORPH_ELLIPSE, cv::Size( size , size ) );
}

void OpenCV::LightDetector::setKernelClose( int size ) {
	mKernelClose = cv::getStructuringElement( cv::MORPH_ELLIPSE, cv::Size( size , size ) );
}

bool Light2DRadiusCompare ( OpenCV::TrackedLight i, OpenCV::TrackedLight j ) {
	return ( i.radius > j.radius );
}

osg::Vec4 OpenCV::LightDetector::getAverageFrameColor() {
	return mFrameMeanColor;
}

void OpenCV::LightDetector::ProcessFrame(cv::Mat& frame , cv::Mat frameGray )
{
	// circle mask
	cv::Mat mask( frameGray.size(), CV_8U, cv::Scalar( 255 ) );
	cv::circle( mask, this->mfisheyeCenter, this->mFisheyeRadius, cv::Scalar( 0 ), CV_FILLED );

	// mask grey
	frameGray.setTo( cv::Scalar( 0 ), mask );

	// average frame color
	cv::Scalar mean = cv::mean( frame, mask );
	mean /= 255;
	//qDebug() << "image mean " << mean.val[0] << " " << mean.val[1] << " " << mean.val[2] << " " << mean.val[3] << " " ;
	mFrameMeanColor.r() = static_cast< float > ( mean[0] );
	mFrameMeanColor.g() = static_cast< float > ( mean[1] );
	mFrameMeanColor.b() = static_cast< float > ( mean[2] );
	//mFrameMean.a() = static_cast< float > ( mean[3] );
	mFrameSurface = M_PI * this->mFisheyeRadius * this->mFisheyeRadius;


	// threshold - get the bright spots
	cv::threshold( frameGray, frameGray, 230, 255, cv::THRESH_BINARY );

	// clean up thresholded image
	cv::morphologyEx( frameGray, frameGray, cv::MORPH_OPEN, this->mKernelOpen, cv::Point( -1, -1 ), 1 ) ;
	cv::morphologyEx( frameGray, frameGray, cv::MORPH_CLOSE, this->mKernelClose, cv::Point( -1, -1 ), 1 );

	// reset data
	mContours.clear();
	mLightCount = 0;

	// find separate contours
	cv::findContours( frameGray.clone(), mContours, cv::RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point( 0, 0 ) );

	// enclosing circles for contours
	for ( int i = 0; i < mContours.size(); ++i ) {
		if ( mLights.size() <= i)
			mLights.push_back( OpenCV::TrackedLight() );

		cv::minEnclosingCircle(mContours[i], mLights[i].positionFrame, mLights[i].radius );
		//qDebug() << "center x " << mLights2D[i].first.x << " y " << mLights2D[i].first.y << " r " << mLights2D[i].second;
		mLightCount++;
	}

	// only 8 lights
	if (mLightCount > 7)
		mLightCount = 7;

	//qDebug() << "count " << mLightCount;
	//qDebug() << "=======";

	std::sort ( mLights.begin(), mLights.end(), Light2DRadiusCompare);

	// calculate position on hemisphere
	for ( int i = 0; i < 8; i++ ) {
		mLights[i].extractColor( frame );
		mLights[i].findIntensity( mFrameSurface );
		mLights[i].mapFrameToHemishere( mfisheyeCenter, mFisheyeRadius );
		//qDebug() << "center x " << mLights[i].hemispherePosition.x() << " y " << mLights[i].hemispherePosition.y() << " z " << mLights[i].hemispherePosition.z() << " r " << mLights[i].radius;
	}
}

int OpenCV::LightDetector::getLightNum() {
	return mLightCount;
}

OpenCV::TrackedLight OpenCV::LightDetector::getLight( int index )
{
	mLights[index].id = index;
	if ( mLightCount > index ) {
		mLights[index].active = true;
		//qDebug() << "center x " << mLights[index].hemispherePosition.x() << " y " << mLights[index].hemispherePosition.y() << " z " << mLights[index].hemispherePosition.z() << " r " << mLights[index].radius;
	} else {
		mLights[index].active = false;
	}
	return mLights[index];
}

