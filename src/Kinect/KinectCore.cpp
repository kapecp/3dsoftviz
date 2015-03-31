#include "Kinect/KinectCore.h"
#include "QDebug"

#include "Kinect/KinectThread.h"
#include "Kinect/KinectWindow.h"
#include "QOpenCV/OpenCVWindow.h"

#include "Core/Core.h"
#include "Viewer/CameraManipulator.h"

Kinect::KinectCore* Kinect::KinectCore::mKinectCore;

Kinect::KinectCore::KinectCore( QApplication* app,QWidget* parent )
{
    mKinectCore = this;
    mParent=parent;
    this->app=app;

    mOpencvWindow=NULL;

    mThrsCreated=false;
    mThrKinect=NULL;
}

Kinect::KinectCore::~KinectCore()
{
    if ( mThrsCreated ) {
        mThrKinect->setCancel( true );
        mThrKinect->setImageSend( false );

        mThrKinect->wait();
        mThrKinect->deleteLater();
    }
}

Kinect::KinectCore* Kinect::KinectCore::getInstance( QApplication* app,QWidget* parent )
{
    // if Kinect exists
    if ( mKinectCore == NULL ) {
        mKinectCore = new Kinect::KinectCore( app,parent );
    }
    return mKinectCore;
}

