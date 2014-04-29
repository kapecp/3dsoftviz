
#ifndef VIEWER_CAMERA_STREAM_DEF
#define VIEWER_CAMERA_STREAM_DEF 1

#include <QObject>
#include <osg/Image>
#include <osg/Geometry>

#ifdef OPENCV_FOUND
#include <opencv2/core/core.hpp>

#endif

namespace OpenCV{

/**
* @brief Class CameraStream
* @author Dávid Durčák
* @date 14.04.2014
*/
class CameraStream : public QObject, public osg::Image
{
	Q_OBJECT

public:

	/**
		* @author Dávid Durčák
		* @brief CameraStream constructor
		* @param geom Geometry for updating it's vertexArray according ratio of image
		*/
	CameraStream( osg::Geometry *geom = NULL);
	~CameraStream();


public slots:

	/**
		* @author Dávid Durčák
		* @brief updateBackgroundImage Slot set cvImg as internal image data and call dirty() and updateGeometryCoords()
		* @param cvImg opencv image
		*/
#ifdef OPENCV_FOUND
	void updateBackgroundImage(cv::Mat cvImg);
#endif

private:

	/**
		* @author Dávid Durčák
		* @brief updateGeometryCoords Update mGeom's vertexArray according ratio of image, given by width and height
		* @param width image' width
		* @param height image' height
		*/
	void updateGeometryCoords(int width, int height);

	int				mWidth;
	int				mHeight;
	osg::Geometry	*mGeom;
	IplImage		*iplImg;
};
}

#endif
