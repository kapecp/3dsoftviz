#include "Kinect/KinectCore.h"
#include "QDebug"

#include "Kinect/KinectThread.h"
#include "Kinect/KinectWindow.h"

#include "Core/Core.h"
#include "Viewer/CameraManipulator.h"

using namespace Kinect;
//already in Aruco, been defined
//Q_DECLARE_METATYPE(cv::Mat)

Kinect::KinectCore * Kinect::KinectCore::mKinectCore;

Kinect::KinectCore::KinectCore( QApplication* app,QWidget *parent)
{
	mKinectCore = this;
	mParent=parent;
	this->app=app;

	mKinectDialog=NULL;

	mThrsCreated=false;
	mThrKinect=NULL;

	//qRegisterMetaType<cv::Mat>("Mat");
}

Kinect::KinectCore::~KinectCore()
{
	if(mThrsCreated)
	{
		mThrKinect->setCancel(true);
		mThrKinect->setImageSend(false);

		mKinectDialog->disconnect();
		mKinectDialog->deleteLater();

		mThrKinect->wait();
		mThrKinect->deleteLater();
	}
}

void Kinect::KinectCore::kinectRecognition()
{
	if(!mKinectDialog)
	{
		if(!mThrsCreated)
		{
			qDebug() << "Kinect Thread";
			mThrsCreated=true;
			mThrKinect = new Kinect::KinectThread();
		}
		qDebug()<< "create Kinect Window";
		mKinectDialog= new Kinect::KinectWindow(mParent,app,mThrKinect);
	}
	mKinectDialog->show();
	createConnectionKinect();
}


Kinect::KinectCore * Kinect::KinectCore::getInstance( QApplication* app,QWidget *parent)
{
	// if Kinect exists
	if(mKinectCore == NULL)
	{
		mKinectCore = new Kinect::KinectCore(app,parent);
	}
	return mKinectCore;
}

void Kinect::KinectCore::createConnectionKinect()
{

	//for video sending
	QObject::connect(mKinectDialog,
					 SIGNAL(sendImageKinect(bool)),
					 mThrKinect,
					 SLOT(setImageSend(bool)));

	//send image to label
	QObject::connect(mThrKinect,
					 SIGNAL(pushImage(cv::Mat)),
					 mKinectDialog,
					 SLOT(setLabel(cv::Mat)));

	//start
	QObject::connect(mKinectDialog,
					 SIGNAL(startKinect()),
					 mThrKinect,
					 SLOT(start()));

	//stop
	QObject::connect(mKinectDialog,
					 SIGNAL(stopKinect(bool)),
					 mThrKinect,
					 SLOT(setCancel(bool)));

	// moving camera with gesture
	QObject::connect( mThrKinect,
					  SIGNAL(sendSliderCoords(float,float,float)),
					  AppCore::Core::getInstance(NULL)->getCoreWindow()->getCameraManipulator(),
					  SLOT(setRotationHeadKinect(float,float,float)) );

	//enable/disable cursor movement
	QObject::connect(mKinectDialog,
					 SIGNAL(setMovementCursor(bool)),
					 mThrKinect,
					 SLOT(setCursorMovement(bool)));

	//edit for speed movement
	QObject::connect(mKinectDialog,
					 SIGNAL(sendSpeedKinect(double)),
					 mThrKinect,
					 SLOT(setSpeedKinect(double)));

}
