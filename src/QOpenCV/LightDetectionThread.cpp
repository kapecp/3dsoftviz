#include "QOpenCV/LightDetectionThread.h"


#include "OpenCV/CapVideo.h"
#include "OpenCV/LightDetector.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <QDebug>

/*QOpenCV::LightDetectionThread::LightDetectionThread( QObject* parent )
        : QThread( parent )
{
    mCapVideo		= NULL;
    mCancel			= false;
    mSendImgEnabled	= true;
}*/


/*QOpenCV::LightDetectionThread::~LightDetectionThread( void )
{
}*/

// in a loop, capture frame from camera and detect faces
// send eyes coordinates to change view
void QOpenCV::LightDetectionThread::run()
{
    mCancel = false;
    cv::Mat image;
    //OpenCV::LightDetector*	mLightDetector = new OpenCV::LightDetector();

    if ( mCapVideo == NULL ) {
        qDebug() << "[FaceRecognitionThread::run()]  Camera is not set";
        //delete mLightDetector;
        return;
    }

    while ( !mCancel ) {
        // get image from camera
        image = mCapVideo->queryFrame();

        cv::cvtColor( image, image, CV_BGR2RGB );

        // we detect faces on grayscale image
        //mFaceRecognizer->detectFaces( mCapVideo->getGrayframe() );
        //mFaceRecognizer->annotateFaces( image );

        cv::flip( image, image, 1 );

        // show image
        if ( mSendImgEnabled && !image.empty() ) {
            if ( image.data ) {
                //emit pushImage( image.clone() );    // ???
            }
        }

        msleep( 80 );
    }
    mCapVideo->release();
    mCapVideo = NULL;
    //delete mLightDetector;
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

