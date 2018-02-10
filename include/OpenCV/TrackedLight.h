#ifndef TRACKEDLIGHT_H
#define TRACKEDLIGHT_H
#include "opencv2/core/types_c.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Core/Core.h"
namespace OpenCV {

/**
	 * @author Autor: Marek Karas
	 * @brief The TrackedLight Class for representing tracked lights
	 * @date 11.4.2017
	 */
class TrackedLight
{
public:

	TrackedLight();

	~TrackedLight();

	osg::Vec3			positionHemisphere()		const
	{
		return _positionHemisphere;
	}

	osg::Vec4			color()						const
	{
		return _color;
	}

	float				colorIntensity()			const
	{
		return _colorIntensity;
	}

	float				surface()					const
	{
		return _surface;
	}

	void mapFrameToHemishere( cv::Point2f fisheye_center, float fisheye_radius, const float fisheye_angle );

	void extractColor( cv::Mat& frame );

	void findIntensity( float frame_area, osg::Vec4 frame_color );

	int					id;

	bool				active;

	// cirlce aproximation position
	cv::Point2f			positionFrame;

	float				radius;

	// bounding box
	cv::Rect			bbox;

private:

	static int			unique_id;

	float				_surface;

	osg::Vec3f			_positionHemisphere;

	osg::Vec4f			_color;

	float				_colorIntensity;

	//std::vector<cv::Point3f> history;
};

}
#endif
