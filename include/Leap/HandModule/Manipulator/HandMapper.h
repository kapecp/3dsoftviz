#ifndef HANDMAPPER_H
#define HANDMAPPER_H

#include "Leap.h"
#include <opencv2/core/core.hpp>

#include <osg/Vec3>

namespace Vwr {
class CoreGraph;
}

namespace Softviz {
namespace Leap {

class HandMapper
{

public:

	HandMapper( Vwr::CoreGraph* coreGraph );

	~HandMapper();

	/**
	 * @brief recalculateDepthNode - calibrate offset and adjust hand position
	 * @param vector - palm position tracked by leap
	 * @param diff from z-coodinate 400
	 * @return
	 */
	::Leap::Vector recalculateDepthNode( ::Leap::Vector vector, float diff );
	/**
	 * @brief setNodeScreenCoords - set coordinates of model hand palm
	 * @param nodeScreenCoords
	 */
	void setNodeScreenCoords( osg::Vec3 nodeScreenCoords );


private:
	float calculateAveragePalmFingerDistance( cv::vector<cv::Point> pointList,
			int imageWidth, int imageHeight );
	cv::vector<cv::Point> getHandPointList( cv::vector<cv::vector<cv::Point>> contourPointList );

	/**
	 * @brief coreGraph -core graph
	 */
	Vwr::CoreGraph* coreGraph;
	/**
	 * @brief cameraOffset - calibrated offset of hand model palm and backgroud image palm
	 */
	cv::Point cameraOffset;
	/**
	 * @brief nodeScreenCoords - position of model hand palm
	 */
	osg::Vec3 nodeScreenCoords;
	/**
	 * @brief calibrationCounter - counter necessary for calibration
	 */
	unsigned long calibrationCounter;
	/**
	 * @brief calibrationLastPoint - last position of hand model palm
	 */
	cv::Point calibrationLastPoint;

};

} // namespace Leap
} // namespace Softviz

#endif // HANDMAPPER_H
