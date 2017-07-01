#ifndef LIGHTDETECTOR_H
#define LIGHTDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <osg/Vec4d>
#include <OpenCV/TrackedLight.h>


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

	void DrawLightCircles( cv::Mat src );

	void setFisheyeAngle( int angle );

	void setFisheyeCenter( cv::Point );

	void setFisheyeRadius( int );

	void setKernelOpen( int size );

	void setKernelClose( int size );

	void ProcessFrame( cv::Mat& frame, cv::Mat frameGray );

	OpenCV::TrackedLight getLight( int index );

	// simple getters

	osg::Vec4 getAverageColor() const
	{
		return mFrameMeanColor;
	}

	float getAverageColorIntensity() const
	{
		return mFrameMeanColorIntensity;
	}

	int getLightNum() const
	{
		return mLightCount;
	}

private:

	void calculateSurface();

	cv::Mat											mKernelOpen;

	cv::Mat											mKernelClose;

	cv::Point2f										mfisheyeCenter;

	float											mFisheyeRadius;

	float											mFisheyeAngle;

	QVector< OpenCV::TrackedLight >					mLights;

	int												mLightCount;

	osg::Vec4										mFrameMeanColor;

	float											mFrameMeanColorIntensity;

	float											mFrameSurface;

	std::vector< std::vector< cv::Point > >			mContours;
};
}

#endif // LIGHTDETECTOR_H
