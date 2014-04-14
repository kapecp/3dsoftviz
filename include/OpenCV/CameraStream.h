
#ifndef VIEWER_CAMERA_STREAM_DEF
#define VIEWER_CAMERA_STREAM_DEF 1

#include <QObject>
#include <osg/Image>
#include <osg/Geometry>
#include <opencv2/core/core.hpp>



namespace OpenCV{

class CameraStream : public QObject, public osg::Image
{
	Q_OBJECT

public:

	CameraStream( osg::Geometry *geom = NULL);
	~CameraStream();


public slots:

	void updateBackgroundImage(cv::Mat cvImg);


private:

	void updateGeometryCoords(int width, int height);

	int				mWidth;
	int				mHeight;
	osg::Geometry	*mGeom;

};
}

#endif
