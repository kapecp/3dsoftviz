#include "Kinect/KinectCore.h"

using namespace Kinect;


Kinect::KinectCore * Kinect::KinectCore::mKinectCore;

Kinect::KinectCore::KinectCore( QApplication* app)
{
    mKinectCore = this;
    this->app=app;
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
