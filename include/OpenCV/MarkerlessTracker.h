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

	MarkerlessTracker();
	~MarkerlessTracker();

	/**
	 * @author Autor: Lukas Hagara
	 * @brief track white circles on image
	 * @param img to track objects on
	 */
    void track(cv::Mat& frame );

private:
    /**
     * @author Autor: Juraj Marak
     */
    ArucoModul::ArControlClass* mArControlClass;
    ArucoModul::ArucoCore* aCore;
    aruco::CameraParameters mCamParam;
	std::vector<TrackerBall> tBalls;

    /**
     * @author Autor: Juraj Marak
     * @brief detect circles in frame
     */
	void findCirclesInFrame(cv::Mat& frame);
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
	 * @param edge img
	 */
	std::vector<cv::Point2f> getPointPositions(cv::Mat binaryImage);
	/**
	 * @author Autor: Marek Rostar
	 * @brief calculate center and radius for circle fited through 3 points from edge img
	 * @param points from edge img, center radius
	 */
	inline void getCircle(cv::Point2f& p1,cv::Point2f& p2,cv::Point2f& p3, cv::Point2f& center, float& radius);
	/**
	 * @author Autor: Marek Rostar
	 * @brief Chceck the inlier count in fitted circle
	 * @param edge img center, radius and found inliers
	 */
	float verifyCircle(cv::Mat dt, cv::Point2f center, float radius, std::vector<cv::Point2f> & inlierSet);
//OpenCV::MarkerlessTracker::
};
}

#endif // MARKERLESSTRACKER_H
