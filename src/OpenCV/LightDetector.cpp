#include "OpenCV/LightDetector.h"

#include <opencv2/core/core_c.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <QDebug>

// constructor loads classifier file with learned faces and set start parameters
OpenCV::LightDetector::LightDetector()
{
    mfisheyeCenter =  cv::Point(256,256);
    mFisheyeRadius = 256;
    mKernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
}

OpenCV::LightDetector::~LightDetector()
{
}

void OpenCV::LightDetector::CircleMask( cv::Mat &src )
{
    cv::Mat out = src.clone();
    cv::Mat mask = cv::Mat::zeros( src.size(), CV_8U );
    cv::circle(out, this->mfisheyeCenter, this->mFisheyeRadius, CV_RGB(255,255,255), CV_FILLED);
    out.copyTo(src, mask);
}

void OpenCV::LightDetector::DrawBoundary( cv::Mat src )
{
    cv::circle(src, this->mfisheyeCenter, this->mFisheyeRadius, CV_RGB(255,0,0));
}

void OpenCV::LightDetector::DrawLightContours( cv::Mat src )
{
    cv::RNG rng;
    for (uint i = 0; i< mContours.size(); i++)
    {
        //printf("contour:%d  size:%.2lf pos: x:%.2lf y:%.2lf, projected: x:%.2lf y:%.2lf z:%.2lf\n", i, contourArea(contours[i]), mc[i].x,mc[i].y, est_lights[i].x, est_lights[i].y, est_lights[i].z);
        cv::Scalar color = cv::Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        cv::drawContours( src, mContours, static_cast< int > ( i ) , color, 2, 8, cv::noArray(), 0, cv::Point() );
    }
}

void OpenCV::LightDetector::setFisheyeCenter( cv::Point2d center )
{
    mfisheyeCenter = cv::Point2d( center.x, center.y );
}

void OpenCV::LightDetector::setFisheyeRadius( int radius )
{
    mFisheyeRadius = static_cast< double > ( radius );
}

void OpenCV::LightDetector::ProcessFrame(cv::Mat &frame )
{
    //CircleMask(frame);

	//cv::Mat out = frame.clone();
	//cv::Mat mask = cv::Mat::zeros( frame.size(), CV_8U );
	//cv::circle(out, this->mfisheyeCenter, this->mFisheyeRadius, CV_RGB(255,255,255), CV_FILLED);
	//out.copyTo(frame, mask);

    // threshold - get the bright spots
    cv::threshold( frame, frame, 230, 255, cv::THRESH_BINARY );

    // clean up thresholded image
    //cv::morphologyEx( frame, frame, cv::MORPH_OPEN, this->mKernel, cv::Point( -1, -1 ), 1 ) ;
    //cv::morphologyEx( frame, frame, cv::MORPH_CLOSE, this->mKernel, cv::Point( -1, -1 ), 1 );

    // reset data
    mContours.clear();
    mLights3D.clear();
    mMoments.clear();

    // find separate contours
    cv::findContours(frame, mContours, cv::RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

    for ( uint i = 0; i < mContours.size(); i++ )
    {
        mMoments.push_back(cv::moments(mContours[i], true));
        //mu[i] = moments(contours[i], false);
    }

    cv::Point2d offset;
    //  Get the mass centers:
    for ( uint i = 0; i < mContours.size(); i++)
    {
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

