#ifndef MARKERLESSTRACKER_H
#define MARKERLESSTRACKER_H

#include <opencv2/opencv.hpp>
#include "Aruco/arControlObject.h"
#include "Aruco/ArucoCore.h"
#include "OpenCV/TrackerBall.h"
#include <aruco/aruco.h>


namespace ArucoModul {
class ArControllClass;
class ArucoCore;
}
namespace OpenCV {

class MarkerlessTracker;

/**
	 * @author Autor: Lukas Hagara
	 * @brief The MarkerlessTracker class
	 * @date 24.2.2017
	 */
class MarkerlessTracker : public QObject
{

public:

	MarkerlessTracker( ArucoModul::ArControlClass* arControlClass );
	~MarkerlessTracker();

	/**
	 * @author Autor: Lukas Hagara
	 * @brief track white circles on image
	 * @param img to track objects on
	 */
	void track( cv::Mat& frame );

private:
	/**
	 * @author Autor: Juraj Marak
	 */

	/**
	 * @brief variable with control class controlling certain aspects of nodes in graph through aruco
	 */
	ArucoModul::ArControlClass* mArControlClass;
	/**
	 * @brief variable with core parts of aruco, responsible for processing the image
	 */
	ArucoModul::ArucoCore* aCore;

	/**
	 * @brief parameters of camera for aruco
	 */
	aruco::CameraParameters mCamParam;

	/**
	 * @brief vector of tracker balls used for markerless tracking
	 */
	std::vector<TrackerBall> tBalls;

	/**
	 * @brief number of current frame used for optimalization, by running RANSAC only in certain frames
	 */
	int framenum;


	/**
	 * @author Autor: Juraj Marak
	 * @brief detect circles in frame
	 * @param cv::Mat to find circles in
	 */
	void findCirclesInFrame( cv::Mat& frame );

	//OpenCV::MarkerlessTracker::
	/**
	 * @author Autor: Juraj Marak
	 * @brief estimate pose of circle based on frame position
	 */
	void estimatePoseCircle( osg::Vec2f framePosition,
							 float circleRadiusReal,
							 float circleRadiusFrame,
							 aruco::CameraParameters mCamParam,
							 cv::Mat& rvec, cv::Mat& tvec
						   );
	//OpenCV::MarkerlessTracker::
	/**
	 * @author Autor: Autor: Juraj Marak
	 * @brief calculate MVMatrix from estimated pose
	 */
	void calculateMVMatrixFromPose( cv::Mat rvec, cv::Mat tvec,
									QMatrix4x4& mVMatrix
								  );

	/**
	 * @author Autor: Marek Rostar
	 * @brief get positions of points  edge image
	 * @param cv::Mat containing edge img
	 * @return std::vector of cv::Point2f containing edge positions of points
	 */
	std::vector<cv::Point2f> getPointPositions( cv::Mat binaryImage );
	/**
	 * @author Autor: Marek Rostar
	 * @brief calculate center and radius for circle fited through 3 points from edge img
	 * @param points from edge img
	 * @param center
	 * @param radius
	 */
	inline void getCircle( cv::Point2f& p1,cv::Point2f& p2,cv::Point2f& p3, cv::Point2f& center, float& radius );
	/**
	 * @author Autor: Marek Rostar
	 * @brief Chceck the inlier count in fitted circle
	 * @param edge img
	 * @param center
	 * @param radius
	 * @param found inliers
	 * @return float  percentage of inliers of fitted circle
	 */
	float verifyCircle( cv::Mat dt, cv::Point2f center, float radius, std::vector<cv::Point2f>& inlierSet );
//OpenCV::MarkerlessTracker::
};
}

#endif // MARKERLESSTRACKER_H
