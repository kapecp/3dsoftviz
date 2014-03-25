
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
	Kinect::KinectRecognition *mKinect = new Kinect::KinectRecognition();

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
	color.create(mKinect->device, openni::SENSOR_COLOR);
	color.start();
	openni::VideoFrameRef colorFrame;


	//nite::HandTracker handTracker;
	//handTracker.create();
	nite::HandTrackerFrameRef handTrackerFrame;
	openni::VideoFrameRef depthFrame;

	m_pHandTracker.create(&mKinect->device);
	m_pHandTracker.startGestureDetection(nite::GESTURE_WAVE);
	m_pHandTracker.startGestureDetection(nite::GESTURE_CLICK);

	while(!mCancel)
	{
		color.readFrame(&colorFrame);
		frame=mKinect->colorImageCvMat(colorFrame);

		///////////////Depth test///////////
		//handTracker.readFrame(&handTrackerFrame);
		//frame = mKinect->depthImageCvMat(handTrackerFrame);

		// cita handframe, najde gesto na snimke a vytvori mu "profil"
		m_pHandTracker.readFrame(&handTrackerFrame);
		const nite::Array<nite::GestureData>& gestures = handTrackerFrame.getGestures();
		for (int i = 0; i < gestures.getSize(); ++i)
		{
			if (gestures[i].isComplete())
			{
				const nite::Point3f& position = gestures[i].getCurrentPosition();
				printf("Gesture %d at (%f,%f,%f)\n", gestures[i].getType(), position.x, position.y, position.z);

				nite::HandId newId;
				m_pHandTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
			}
		}

		// List of hands evidence
		// If hand matches old positions, previous ID is assigned again
		const nite::Array<nite::HandData>& hands= handTrackerFrame.getHands();
		for (int i = 0; i < hands.getSize(); ++i)
		{
			const nite::HandData& user = hands[i];

			if (!user.isTracking())
			{
				printf("Lost hand %d\n", user.getId());
				nite::HandId id = user.getId();
				HistoryBuffer<20>* pHistory = g_histories[id];
				g_histories.erase(g_histories.find(id));
				delete pHistory;
			}
			else
			{
				if (user.isNew())
				{
					printf("New hand id %d\n", user.getId());
					g_histories[user.getId()] = new HistoryBuffer<20>;
				}
				// Hand evidence in Buffer
				HistoryBuffer<20>* pHistory = g_histories[user.getId()];
				pHistory->AddPoint(user.getPosition());
				// Data for mouse
				printf("user %d %.2lf %.2lf\n", user.getId(), user.getPosition().x, user.getPosition().y);
			}
		}

		//////////////End/////////////

		//	cap >> frame; // get a new frame from camera
		cv::cvtColor(frame, frame, CV_BGR2RGB);
		//QImage qimage ( (uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

		///////Emit Qimage and Image////////////
		//emit pushImage( qimage );
		emit pushImage( frame );

	}
	//testing Camera//////////////
	//cap.release();

}




