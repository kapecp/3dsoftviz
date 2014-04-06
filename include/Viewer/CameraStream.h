
#ifndef VIEWER_CAMERA_STREAM_DEF
#define VIEWER_CAMERA_STREAM_DEF 1

#include <QObject>
#include <osg/ImageStream>
#include <opencv2/core/core.hpp>



namespace Vwr{

class CameraStream : public QObject, public osg::ImageStream
{
	Q_OBJECT

public:
	CameraStream();
	~CameraStream();

public slots:
	void updateBackgroundImage(cv::Mat cvImg);

};
}

#endif
