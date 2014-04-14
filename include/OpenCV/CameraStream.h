
#ifndef VIEWER_CAMERA_STREAM_DEF
#define VIEWER_CAMERA_STREAM_DEF 1

#include <QObject>
#include <osg/Image>
#include <osg/Geometry>

#ifdef OPENCV_FOUND
#include <opencv2/core/core.hpp>
#endif


namespace OpenCV{

class CameraStream : public QObject, public osg::Image
{
	Q_OBJECT

public:

	CameraStream( osg::Geometry *geom = NULL);
	~CameraStream();


public slots:

#ifdef OPENCV_FOUND
	void updateBackgroundImage(cv::Mat cvImg);
#endif

private:

	void updateGeometryCoords(int width, int height);

	int				mWidth;
	int				mHeight;
	osg::Geometry	*mGeom;

};
}

#endif
