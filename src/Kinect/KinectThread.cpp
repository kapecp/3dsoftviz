
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
	openni::VideoStream  m_depth;
	openni::CoordinateConverter coordinateConverter;
	// convert milimeters to pixels
	float pDepth_x;
	float pDepth_y;
	float pDepth_z;
	float pDepth_x2;
	float pDepth_y2;
	float pDepth_z2;
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
		m_depth.create(mKinect->device, openni::SENSOR_DEPTH);
		m_depth.start();

		kht = new KinectHandTracker(&mKinect->device,&m_depth);
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
		if (kht->isTwoHands == true)
		{
			coordinateConverter.convertWorldToDepth(m_depth, kht->getArrayHands[0][0], kht->getArrayHands[0][1], kht->handZ[0], &pDepth_x, &pDepth_y, &pDepth_z);
			coordinateConverter.convertWorldToDepth(m_depth, kht->getArrayHands[1][0], kht->getArrayHands[1][1], kht->handZ[1], &pDepth_x2, &pDepth_y2, &pDepth_z2);

			pDepth_y = kht->handTrackerFrame.getDepthFrame().getHeight() - pDepth_y;
			pDepth_y2 = kht->handTrackerFrame.getDepthFrame().getHeight() - pDepth_y2;

			//printf("depth X, Y, Z: %f %f %f\n",pDepth_x,pDepth_y,pDepth_z);

			Rect hand_rect;

			if (pDepth_x < pDepth_x2) hand_rect.x = pDepth_x;
			else hand_rect.x = pDepth_x2;
			if (pDepth_y < pDepth_y2) hand_rect.y = pDepth_y;
			else hand_rect.y = pDepth_y2;

			hand_rect.width = abs(pDepth_x - pDepth_x2);
			hand_rect.height = abs(pDepth_y - pDepth_y2);//kht->handY[1] - kht->handY[0];

			rectangle(frame, hand_rect, CV_RGB(0, 255,0), 3);
		}

		cv::resize(frame, frame,cv::Size(320,240),0,0,cv::INTER_LINEAR);
		emit pushImage( frame );
		msleep(20);

	}
	//testing Camera//////////////
	//cap.release();

}




