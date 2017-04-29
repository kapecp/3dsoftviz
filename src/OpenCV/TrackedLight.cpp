#include "OpenCV/TrackedLight.h"


int OpenCV::TrackedLight::unique_id = 0;

OpenCV::TrackedLight::TrackedLight()
{
	id = unique_id;
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
}
