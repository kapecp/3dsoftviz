#include "OpenCV/LightDetector.h"
#include <osg/Vec4d>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
//#include <opencv\cv.hpp>

#include <opencv2/features2d/features2d.hpp>

#include <QDebug>

// constructor loads classifier file with learned faces and set start parameters
OpenCV::LightDetector::LightDetector()
{
	for (int i = 0; i < 8; i++)
		mLights.push_back( TrackedLight() );
}

OpenCV::LightDetector::~LightDetector()
{
}

void OpenCV::LightDetector::CircleMask( cv::Mat& src, cv::Point center, int radius )
{
	cv::Mat mask( src.size(), CV_8U, cv::Scalar( 255 ) );
	cv::circle( mask, center, radius, cv::Scalar( 0 ), CV_FILLED );
	src.setTo( cv::Scalar( 0 ), mask );
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
		cv::circle(src, mLights[i].framePosition, (int) mLights[i].radius , CV_RGB( 0, 255, 0 ) );
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

void OpenCV::LightDetector::ProcessFrame( cv::Mat& frame )
{
	CircleMask( frame, this->mfisheyeCenter, this->mFisheyeRadius );

	// threshold - get the bright spots
	cv::threshold( frame, frame, 230, 255, cv::THRESH_BINARY );

	// clean up thresholded image
	cv::morphologyEx( frame, frame, cv::MORPH_OPEN, this->mKernelOpen, cv::Point( -1, -1 ), 1 ) ;
	cv::morphologyEx( frame, frame, cv::MORPH_CLOSE, this->mKernelClose, cv::Point( -1, -1 ), 1 );

	// reset data
	mContours.clear();
	mLightCount = 0;

	// find separate contours
	cv::findContours( frame.clone(), mContours, cv::RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point( 0, 0 ) );

	// enclosing circles for contours
	for ( int i = 0; i < mContours.size(); ++i ) {
		if ( mLights.size() <= i)
			mLights.push_back( OpenCV::TrackedLight() );

		cv::minEnclosingCircle(mContours[i], mLights[i].framePosition, mLights[i].radius );
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
		mLights[i].MapToHemisphere( mfisheyeCenter, mFisheyeRadius );
		mLights[i].active = (i <= mLightCount);
		//qDebug() << "center x " << mLights[i].hemispherePosition.x() << " y " << mLights[i].hemispherePosition.y() << " z " << mLights[i].hemispherePosition.z() << " r " << mLights[i].radius;
	}
}

int OpenCV::LightDetector::getLightNum() {
	return mLightCount;
}

OpenCV::TrackedLight OpenCV::LightDetector::getLight( int index )
{
	if ( mLightCount > index ) {
		//qDebug() << "center x " << mLights[index].hemispherePosition.x() << " y " << mLights[index].hemispherePosition.y() << " z " << mLights[index].hemispherePosition.z() << " r " << mLights[index].radius;
		return mLights[index];
	}
	return OpenCV::TrackedLight();
}

