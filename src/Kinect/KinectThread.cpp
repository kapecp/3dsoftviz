
#include "Kinect/KinectThread.h"
#include "Kinect/KinectCore.h"
#include "Kinect/KinectRecognition.h"
#include "QDebug"

//only for testing
#include "opencv/highgui.h"
#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"
#include "OpenNI.h"
#include "NiTE.h"


using namespace Kinect;

//for testing
using namespace cv;

Kinect::KinectThread::KinectThread(QObject *parent) : QThread(parent)
{
	mCancel=false;
}

Kinect::KinectThread::~KinectThread(void)
{
}

void Kinect::KinectThread::setCancel(bool set)
{
	mCancel=set;
}

void Kinect::KinectThread::pause()
{
	mCancel=true;
}

void Kinect::KinectThread::run()
{

	mCancel=false;
	/////Class kinect recognition
	//Kinect::KinectRecognition *mKinect = new Kinect::KinectRecognition();
	//openni::VideoStream  color;
	/////////end////////////

	cv::Mat frame;

	//////////////for Testing
	cv::VideoCapture cap(0);
	if(!cap.isOpened())
	{
		qDebug() << "No camera";
		exit();
	}
	//////////////Koniec testovania//////////////////

	//////////////Kinect ///////
	//	color.create(mKinect->device, openni::SENSOR_COLOR);
	//	color.start();
	//	openni::VideoFrameRef colorFrame;

	//nite::HandTracker handTracker;
	//handTracker.create();
	//nite::HandTrackerFrameRef handTrackerFrame;

	while(!mCancel)
	{
		//color.readFrame(&colorFrame);
		//frame=mKinect->colorImageCvMat(colorFrame);

		///////////////Dept test///////////
		//handTracker.readFrame(&handTrackerFrame);
		//frame = mKinect->depthImageCvMat(handTrackerFrame);
		//////////////End/////////////

		cap >> frame; // get a new frame from camera
		cv::cvtColor(frame, frame, CV_BGR2RGB);
		//QImage qimage ( (uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

		///////Emit Qimage and Image////////////
		//emit pushImage( qimage );
		emit pushImage( frame );

	}
	//testing Camera//////////////
	cap.release();

}




