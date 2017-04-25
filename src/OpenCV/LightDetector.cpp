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
	mfisheyeCenter =  cv::Point( 256,256 );
	mFisheyeRadius = 256;
	mFisheyeDisrotion = 1.0;
	mKernel = cv::getStructuringElement( cv::MORPH_ELLIPSE, cv::Size( 5, 5 ) );
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

void OpenCV::LightDetector::DrawBoundary( cv::Mat src )
{
	int circles = 5;
	double disort = 2;
	double frac = pow( disort, circles );
	double tmp;
	qDebug() << "radius " << this->mFisheyeRadius;

	for ( int i = 1; i <= circles; ++i ) {
		tmp = (double) i / circles * this->mFisheyeRadius;
		qDebug() << "i " << i << " tmp " << tmp;
		cv::circle( src, this->mfisheyeCenter, (int)tmp, CV_RGB( 255,0,0 ) );
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

void OpenCV::LightDetector::setFisheyeCenter( cv::Point2d center )
{
	mfisheyeCenter = cv::Point2d( center.x, center.y );
}

void OpenCV::LightDetector::setFisheyeRadius( int radius )
{
	mFisheyeRadius = static_cast< double >( radius );
}

void OpenCV::LightDetector::ProcessFrame( cv::Mat& frame )
{
	/*cv::Mat mask( frame.size(), CV_8U, cv::Scalar( 255 ) );
	cv::circle( mask, this->mfisheyeCenter, this->mFisheyeRadius, cv::Scalar( 0 ), CV_FILLED );
	frame.setTo( cv::Scalar( 0 ), mask );*/

	CircleMask( frame, this->mfisheyeCenter, this->mFisheyeRadius );


	// threshold - get the bright spots
	cv::threshold( frame, frame, 230, 255, cv::THRESH_BINARY );
	cv::imshow( "tresh",frame );

	// clean up thresholded image
	cv::morphologyEx( frame, frame, cv::MORPH_OPEN, this->mKernel, cv::Point( -1, -1 ), 1 ) ;
	cv::morphologyEx( frame, frame, cv::MORPH_CLOSE, this->mKernel, cv::Point( -1, -1 ), 1 );

	cv::imshow( "morph",frame );
	// reset data
	mContours.clear();
	mLights3D.clear();
	mMoments.clear();
	mContourCenters.clear();

	// find separate contours
	cv::findContours( frame.clone(), mContours, cv::RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point( 0, 0 ) );

	for ( uint i = 0; i < mContours.size(); i++ ) {
		mMoments.push_back( cv::moments( mContours[i], true ) );
		//mu[i] = moments(contours[i], false);
	}

	cv::Point2d offset;
	//  Get the mass centers:
	for ( uint i = 0; i < mContours.size(); i++ ) {
		//mc[i] = Point2d(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		mContourCenters.push_back( cv::Point2d( mMoments[i].m10 / mMoments[i].m00, mMoments[i].m01 / mMoments[i].m00 ) );
		offset = mContourCenters[i] - mfisheyeCenter;
		//offset.x = mContourCenters[i].x - mfisheyeCenter.x;
		//offset.x = mContourCenters[i].y - mfisheyeCenter.y;
		offset.x /= mFisheyeRadius;
		offset.y /= mFisheyeRadius;
		mLights3D.push_back( cv::Point3d( offset.x, offset.y, ( 1.0 - ( offset.x * offset.x ) - ( offset.y * offset.y ) ) ) );
	}

	//printf("radius %d\n", shot_radius);

	//LigthEstimation_ProjectLights(mc, est_lights, Point(shot_center_x, shot_center_y), shot_radius);
}

osg::Vec4d OpenCV::LightDetector::getLight( int index )
{
	if ( mLights3D.size() > index ) {
		qDebug() << "light x: " << mLights3D[index].x << " y: " << mLights3D[index].y << " z: " << mLights3D[index].z;
		return osg::Vec4d( mLights3D[index].x, mLights3D[index].y, mLights3D[index].z, 0 );
	}
	return osg::Vec4d( 0, 0, 1, 0 );
}

