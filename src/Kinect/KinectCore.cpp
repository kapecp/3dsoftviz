#include "Kinect/KinectCore.h"
#include "QDebug"

#include "Kinect/KinectThread.h"
#include "Kinect/KinectWindow.h"

using namespace Kinect;
Q_DECLARE_METATYPE(cv::Mat)



Kinect::KinectCore * Kinect::KinectCore::mKinectCore;

Kinect::KinectCore::KinectCore( QApplication* app,QWidget *parent)
{
	mKinectCore = this;
	mParent=parent;
	this->app=app;
	mKinectDialog=NULL;
	mThrsCreated=false;
	mThrKinect=NULL;
	qRegisterMetaType<cv::Mat>("Mat");
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
		createConnectionKinect();
	}

	mKinectDialog->show();
	mThrKinect->start();


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
	QObject::connect(mThrKinect,SIGNAL(pushImage(cv::Mat)),mKinectDialog,SLOT(setLabel(cv::Mat)));


}
