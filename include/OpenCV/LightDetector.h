#ifndef LIGHTDETECTOR_H
#define LIGHTDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <osg/Vec4d>


namespace OpenCV {

class LightDetector;

/**
	*@brief Class LightDetector
	*@author Autor: Marek Karas
	*@date 18.11.2013
	*/
class LightDetector
{

public:

	LightDetector();
	~LightDetector();

	void DrawBoundary( cv::Mat );

	void DrawLightContours( cv::Mat src );

	void Threshold( int, void* );

	void setFisheyeCenter( cv::Point2d );

	void setFisheyeRadius( int );

	void CircleMask( cv::Mat& src, cv::Point center, int radius );

	void ProcessFrame( cv::Mat& frame );

	void ProjectLights( std::vector<cv::Point2d> input, std::vector<cv::Point3d>& output, cv::Point center, int radius, int lens_angle );

	cv::Point3d MapToHemisphere( cv::Point2d offset );

	osg::Vec4d getLight( int index );

private:

	cv::Mat                                 mKernel;

	cv::Mat                                 mProcessedImage;

	cv::Point2d                               mfisheyeCenter;

	double                                     mFisheyeRadius;

	double									mFisheyeDisrotion;

	std::vector< cv::Point3d >              mLights3D;

	std::vector< std::vector< cv::Point > > mContours;

	std::vector< cv::Moments >              mMoments;

	std::vector< cv::Point2d >                mContourCenters;
};
}

#endif // LIGHTDETECTOR_H
