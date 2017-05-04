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

void OpenCV::TrackedLight::extractColor( cv::Mat &frame ) {
	/*int x = positionFrame.x - radius;
	int y = positionFrame.y - radius;
	int w = radius * 2;
	int h = w;

	x = x < 0 ? 0 : x;
	y = y < 0 ? 0 : y;
	w = ( x + w < frame.cols ) ? w : frame.cols - x;
	h = ( y + h < frame.rows ) ? h : frame.rows - y;
	w = w < 0 ? 0 : w;
	h = h < 0 ? 0 : h;

	cv::Rect roi( x, y, w, h);
	cv::Mat roiMAT( frame, roi );*/
	cv::Mat roiMAT( frame, bbox );
	cv::Scalar mean = cv::mean( roiMAT );

	// debug
	//cv::rectangle( frame, roi, mean, -1, 8 , 0 );
	cv::rectangle( frame, bbox, mean, -1, 8 , 0 );
	mean /= 255;

	_color.r() = static_cast < float > ( mean.val[0] );
	_color.g() = static_cast < float > ( mean.val[1] );
	_color.b() = static_cast < float > ( mean.val[2] );

	//qDebug() << "extract color "  << _color.r() << " " << _color.g() << " " << _color.b();
}

void OpenCV::TrackedLight::findIntensity( float frame_area ) {

	_surface = M_PI * radius * radius;
	_colorIntensity = _surface / frame_area;
	//qDebug() << "intensity "  << _colorIntensity << " radius " << radius;
}

void OpenCV::TrackedLight::mapFrameToHemishere( const cv::Point2f fisheye_center, const float fisheye_radius, const float fisheye_angle ) {
	cv::Point2f offset;

	offset = positionFrame - fisheye_center;
	offset.x /= fisheye_radius;
	offset.y /= fisheye_radius;
	offset *= ( float )( fisheye_angle / 180 ) ;

	_positionHemisphere.x() = offset.x;
	_positionHemisphere.y() = offset.y;
	_positionHemisphere.z() = ( 1.0f - ( offset.x * offset.x ) - ( offset.y * offset.y ) );
}
