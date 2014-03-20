#include "Kinect/KinectCore.h"

#include "Kinect/KinectWindow.h"
#include "QDebug"

using namespace Kinect;


Kinect::KinectCore * Kinect::KinectCore::mKinectCore;

Kinect::KinectCore::KinectCore( QApplication* app,QWidget *parent)
{
	mKinectCore = this;
	mParent=parent;
	this->app=app;
	mKinectDialog=NULL;
	mThrsCreated=false;
	mThrKinect=NULL;
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
			//createConnectionKinect();
		}
		qDebug()<< "create Kinect Window";

		mKinectDialog= new Kinect::KinectWindow(mParent,app);

	}

	mKinectDialog->show();



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
	QObject::connect(mThrKinect,SIGNAL(pushImage(cv::Mat)),mKinectDialog,SLOT(Kinect::KinectWindow::setLabel() ));

}
