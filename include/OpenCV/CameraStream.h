
#ifndef VIEWER_CAMERA_STREAM_DEF
#define VIEWER_CAMERA_STREAM_DEF 1

#include <QObject>
#include <osg/Image>
#include <osg/Geometry>

#ifdef OPENCV_FOUND
#include <opencv2/core/core.hpp>

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
		* @param geom Geometry for updating it's vertexArray according ratio of image
		*/
	CameraStream( osg::Geometry* geom = NULL );
	~CameraStream();

    IplImage* getIplImage();


public slots:

	/**
		* @author Dávid Durčák
		* @brief updateBackgroundImage Slot set cvImg as internal image data and call dirty() and updateGeometryCoords()
		* @param cvImg opencv image
		*/
#ifdef OPENCV_FOUND
	void updateBackgroundImage( cv::Mat cvImg );
#endif

private:

	/**
		* @author Dávid Durčák
		* @brief updateGeometryCoords Update mGeom's vertexArray according ratio of image, given by width and height
		* @param width image' width
		* @param height image' height
		*/
	void updateGeometryCoords( int width, int height );

	int				mWidth; // data about cols cv:Mat
	int				mHeight; // data about rows cv:Mat
	osg::Geometry*	mGeom; // Geometry for vertex array and update
	IplImage*		iplImg; // image for background
};
}

#endif
