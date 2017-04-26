#include "OpenCV/TrackedLight.h"


OpenCV::TrackedLight::TrackedLight()
{
	active = 0;
}
OpenCV::TrackedLight::~TrackedLight()
{

}

void OpenCV::TrackedLight::MapToHemisphere( const cv::Point2f fisheye_center, const float fisheye_radius ) {
	cv::Point2f offset;
	offset = framePosition - fisheye_center;
	offset.x /= fisheye_radius;
	offset.y /= fisheye_radius;
	hemispherePosition.x() = offset.x;
	hemispherePosition.y() = offset.y;
	hemispherePosition.z() = ( 1.0f - ( offset.x * offset.x ) - ( offset.y * offset.y ) );
	hemispherePosition.w() = 1.0;
}
