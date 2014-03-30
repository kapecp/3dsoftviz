
#include "Kinect/KinectThread.h"
#include "Kinect/KinectCore.h"
#include "Kinect/KinectRecognition.h"

#include "QDebug"

//only for testing
#include "opencv/highgui.h"
#include <opencv2/core/core.hpp>
#include "opencv2/opencv.hpp"


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

void Kinect::KinectThread::setImageSend(bool set)
{
	mSetImageEnable=set;
}

void Kinect::KinectThread::pause()
{
	mCancel=true;
}

void Kinect::KinectThread::run()
{

	mCancel=false;
	/////Class kinect recognition
	mKinect = new Kinect::KinectRecognition();
	isOpen=mKinect->isOpenOpenni();
	openni::VideoStream  color;
	/////////end////////////

	cv::Mat frame;
	/*
	//////////////for Testing
	cv::VideoCapture cap(0);
	if(!cap.isOpened())
	{
		qDebug() << "No camera";
		exit();
	}
	//////////////Koniec testovania//////////////////
*/
	//////////////Kinect ///////
	if(isOpen)
	{
		color.create(mKinect->device, openni::SENSOR_COLOR);
		color.start();

		kht = new KinectHandTracker(&mKinect->device);
	}


	while(!mCancel && isOpen)
	{
		color.readFrame(&colorFrame);
		frame=mKinect->colorImageCvMat(colorFrame);

		///////////////Depth test///////////
		//handTracker.readFrame(&handTrackerFrame);
		//frame = mKinect->depthImageCvMat(handTrackerFrame);

		// cita handframe, najde gesto na snimke a vytvori mu "profil"
		kht->getAllGestures();
		kht->getAllHands();

		//////////////End/////////////

		//	cap >> frame; // get a new frame from camera
		cv::cvtColor(frame, frame, CV_BGR2RGB);
		//QImage qimage ( (uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

		///////Emit Qimage and Image////////////
		//emit pushImage( qimage );
		Rect hand_rect;
		hand_rect.x = (kht->handX+1024)/4; // prerobit cez videoframeref
		hand_rect.y = ((kht->handY)+768)/4; // prerobit cez handframe.getwidth....

		hand_rect.height = abs(kht->getDistance[1]) + 20;
		hand_rect.width = abs(kht->getDistance[0]) + 20;


		cv::resize(frame, frame,cv::Size(320,240),0,0,cv::INTER_LINEAR);

		rectangle(frame, hand_rect, CV_RGB(0, 255,0), 1);

		emit pushImage( frame );

	}
	//testing Camera//////////////
	//cap.release();

}




