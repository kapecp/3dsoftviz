#ifndef MARKERLESSTRACKER_H
#define MARKERLESSTRACKER_H

#include <opencv2/opencv.hpp>
#include "Aruco/arControlObject.h"
#include "Aruco/ArucoCore.h"
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

    /**
     * @author Autor: Juraj Marak
     * @brief detect circles in frame
     */
	void findCirclesInFrame(cv::Mat& frame,
                                                        cv::vector<cv::Vec3f> &circlesReal
                                                       );
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
//OpenCV::MarkerlessTracker::
};
}

#endif // MARKERLESSTRACKER_H
