#include "Kinect/KinectCore.h"
#include "QDebug"

using namespace Kinect;


Kinect::KinectCore * Kinect::KinectCore::mKinectCore;

Kinect::KinectCore::KinectCore( QApplication* app)
{
	mKinectCore = this;
	this->app=app;
}

void Kinect::KinectCore::kinectRecognition()
{

	qDebug()<< "create Kinect Window";

}


Kinect::KinectCore * Kinect::KinectCore::getInstance( QApplication* app)
{
	// if Kinect exists
	if(mKinectCore == NULL)
	{
		mKinectCore = new Kinect::KinectCore(app);
	}
	return mKinectCore;
}
