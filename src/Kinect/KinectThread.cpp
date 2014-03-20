
#include "Kinect/KinectThread.h"
#include "Kinect/KinectCore.h"
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

void Kinect::KinectThread::pause()
{
	mCancel=true;
}

void Kinect::KinectThread::run()
{

	mCancel=false;


	cv::Mat frame;

	//KinectCore kCore;

	//temporary for testing
	cv::VideoCapture cap(0);
	if(!cap.isOpened())
	{
		qDebug() << "No camera";
		exit();
	}


	while(!mCancel)
	{

		cap >> frame; // get a new frame from camera

		cv::cvtColor(frame, frame, CV_BGR2RGB);
		//QImage qimage ( (uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

		//emit pushImage( qimage );
		emit pushImage( frame );
		//cv::imshow("daco",frame);
		//cv::waitKey(10);

	}
	//testing
	cap.release();

}




