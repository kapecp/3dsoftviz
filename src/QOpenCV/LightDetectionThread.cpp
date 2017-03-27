#include "QOpenCV/LightDetectionThread.h"


#include "OpenCV/CapVideo.h"
#include "OpenCV/LightDetector.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <QDebug>

QOpenCV::LightDetectionThread::LightDetectionThread( QObject* parent )
        : QThread( parent )
{
    mLightDetector  = nullptr;
    mCapVideo		= NULL;
    mCancel			= false;
    mSendImgEnabled	= true;

    mFisheyeR       = 512;
    mFisheyeX       = 256;
    mFisheyeY       = 256;
}



QOpenCV::LightDetectionThread::~LightDetectionThread( void )
{
}

// in a loop, capture frame from camera and detect faces
// send eyes coordinates to change view
void QOpenCV::LightDetectionThread::run()
{
    mCancel = false;
    cv::Mat image;
    /*OpenCV::LightDetector*	*/mLightDetector = new OpenCV::LightDetector();

    if ( mCapVideo == NULL ) {
        qDebug() << "[LightDetectionThread::run()]  Camera is not set";
        delete mLightDetector;
        return;
    }

    qDebug() << "camera height " << mCapVideo->getHeight();
    qDebug() << "camera width " << mCapVideo->getWidth();

    mCapVideo->setCaptureProperties( 640, 480 );

    qDebug() << "camera height " << mCapVideo->getHeight();
    qDebug() << "camera width " << mCapVideo->getWidth();

    while ( !mCancel ) {
        // get image from camera
        image = mCapVideo->queryFrame();


        cv::cvtColor( image, image, CV_BGR2RGB );

        mLightDetector->DrawBoundary( image );
        // we detect faces on grayscale image
        //mFaceRecognizer->detectFaces( mCapVideo->getGrayframe() );
        //mFaceRecognizer->annotateFaces( image );

        cv::flip( image, image, 1 );

        // show image
        if ( mSendImgEnabled && !image.empty() ) {
            if ( image.data ) {
                emit pushImage( image.clone() );    // ???
            }
        }

        if ( mSendBackgrImgEnabled && !image.empty() ) {
            emit pushBackgrImage( image.clone() );
        }

        msleep( 80 );
    }
    mCapVideo->release();
    mCapVideo = NULL;
    delete mLightDetector;
}

void QOpenCV::LightDetectionThread::setCancel( bool set )
{
    mCancel = set;
}

void QOpenCV::LightDetectionThread::setSendImgEnabled( bool sendImgEnabled )
{
    mSendImgEnabled = sendImgEnabled;
}

void QOpenCV::LightDetectionThread::setSendBackgrImgEnabled( bool sendBackgrImgEnabled )
{
    mSendBackgrImgEnabled = sendBackgrImgEnabled;
}

void QOpenCV::LightDetectionThread::setCapVideo( OpenCV::CapVideo* capVideo )
{
    mCapVideo = capVideo;
}

void QOpenCV::LightDetectionThread::setFishEyeCenterX( int offset ) {
    //qDebug() << "X offset = " << offset;
    mFisheyeX = offset;
    if (mLightDetector != nullptr) {
        mLightDetector->setFisheyeCenter( cv::Point(mFisheyeX,mFisheyeY) );
    }
}

void QOpenCV::LightDetectionThread::setFishEyeCenterY( int offset ) {
    mFisheyeY = offset;
    if (mLightDetector != nullptr) {
        mLightDetector->setFisheyeCenter( cv::Point(mFisheyeX,mFisheyeY) );
    }
}

void QOpenCV::LightDetectionThread::setFishEyeRadius( int radius ) {
    mFisheyeR = radius;
    if (mLightDetector != nullptr) {
        mLightDetector->setFisheyeRadius( mFisheyeR );
    }
}
