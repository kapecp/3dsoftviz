
#ifndef VIEWER_CAMERA_STREAM_DEF
#define VIEWER_CAMERA_STREAM_DEF 1

#include <QObject>
#include <osg/Image>
#include <osg/Geometry>
#include <QMutex>

#ifdef OPENCV_FOUND
#include <opencv2/core/core.hpp>
#include "OpenCV/HandTracker.h"

#endif

namespace OpenCV {

/**
* @brief Class CameraStream
* @author Dávid Durčák
* @date 14.04.2014
* @brief Class provides a obect, that cam by uses as dynamic image for texture. When we changed it, all texture that use it will be automaticly
* updated. It is uses for video background functionality. On linux with old osg library(older than 3.0), there cam be performance troubles.
*/
class CameraStream : public QObject, public osg::Image
{
	Q_OBJECT

public:

	/**
		* @author Dávid Durčák
		* @brief CameraStream constructor
		*/
	CameraStream( );
	~CameraStream();
	int getImageWidth();
	int getImageHeight();

	bool calibrated;
	cv::vector<std::pair<cv::Point,double>> palmAndRadiusList;

public slots:

	/**
		* @author Dávid Durčák
	    * @brief updateBackgroundImage Slot set cvImg as internal image data and call dirty()
		* @param cvImg opencv image
	    * @param determines whether to track hands in the image
		*/
#ifdef OPENCV_FOUND
	void updateBackgroundImage( cv::Mat cvImg, bool trackHands );

	void requestCalibration();

#endif

private:

	cv::Mat image;
	HandTracker* tracker;
	QMutex trackMutex;
};
}

#endif
