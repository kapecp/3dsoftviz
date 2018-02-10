#include "OpenCV/TrackedLight.h"
#include <opencv2/core/core.hpp>


int OpenCV::TrackedLight::unique_id = 0;

OpenCV::TrackedLight::TrackedLight()
{
	_color = osg::Vec4( 1, 1, 1, 1 );
	radius = 0;
	//id = unique_id++;
	active = 0;
}

OpenCV::TrackedLight::~TrackedLight()
{

}

void OpenCV::TrackedLight::extractColor( cv::Mat& frame )
{
	cv::Mat roiMAT( frame, bbox );
	cv::Scalar mean = cv::mean( roiMAT );
	// debug
	//cv::rectangle( frame, bbox, mean, -1, 8 , 0 );
	mean /= 255;

	_color.r() = static_cast < float >( mean.val[0] );
	_color.g() = static_cast < float >( mean.val[1] );
	_color.b() = static_cast < float >( mean.val[2] );

	//qDebug() << "extract color "  << _color.r() << " " << _color.g() << " " << _color.b();
}

void OpenCV::TrackedLight::findIntensity( float frame_area, osg::Vec4 frame_color )
{

	_surface = static_cast<float>( M_PI * static_cast<double>( radius ) * static_cast<double>( radius ) );
	_colorIntensity = ( _surface / frame_area ) * 0.8f + 0.2f * std::abs( _color.length() - frame_color.length() );
	//qDebug() << "intensity "  << _colorIntensity << " radius " << radius;
}

void OpenCV::TrackedLight::mapFrameToHemishere( const cv::Point2f fisheye_center, const float fisheye_radius, const float fisheye_angle )
{
	cv::Point2f offset;

	offset = positionFrame - fisheye_center;
	offset.x /= fisheye_radius;
	offset.y /= fisheye_radius;
	offset *= static_cast<float>( fisheye_angle / 180 ) ;

	_positionHemisphere.x() = offset.x;
	_positionHemisphere.y() = offset.y;
	_positionHemisphere.z() = sqrt( 1.0f - ( offset.x * offset.x ) - ( offset.y * offset.y ) );
}
