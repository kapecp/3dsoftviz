#include "Viewer/CameraStream.h"
#include <QDebug>
#include <opencv2/highgui/highgui.hpp>
#include "Util/ApplicationConfig.h"


using namespace Vwr;

CameraStream::CameraStream() : QObject(), osg::ImageStream()
{
	cv::Mat cvImg = cv::Mat::zeros(640,640, CV_8UC3);
	updateBackgroundImage( cvImg);
}

CameraStream::~CameraStream(){}

void CameraStream::updateBackgroundImage( cv::Mat cvImg)
{
	//qDebug() << "camera stream";
	//cv::imshow("camera stream", cvImg);
	//cv::waitKey(0);

	IplImage* iplImg = cvCloneImage( &(IplImage)cvImg);

	setImage( iplImg->width, iplImg->height, 3,
			  GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
			  (BYTE*)( iplImg->imageData),
			  osg::Image::AllocationMode::NO_DELETE, 1);

	dirty();
}


