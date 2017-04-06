#include "QOpenCV/LightDetectionThread.h"


#include "OpenCV/CapVideo.h"
#include "OpenCV/LightDetector.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <QDebug>
#include <osg/Vec4d>

QOpenCV::LightDetectionThread::LightDetectionThread( QObject* parent )
	: QThread( parent )
{
	mLightDetector  = nullptr;
	mCapVideo		= NULL;
	mCancel			= false;
	mSendImgEnabled	= true;
	mShowProcessing = false;
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
	cv::Mat imageGray;
	mLightDetector = new OpenCV::LightDetector();

	if ( mCapVideo == NULL ) {
		qDebug() << "[LightDetectionThread::run()]  Camera is not set";
		delete mLightDetector;
		return;
	}

	mCapVideo->setCaptureProperties( 640, 480 );
	mCapVideo->setAutoExposure( 0.0 );

	int width = mCapVideo->getWidth();
	int height = mCapVideo->getHeight();

	qDebug() << "camera height " << height;
	qDebug() << "camera width " << width;

	mLightDetector->setFisheyeCenter( cv::Point( width / 2 , height / 2 ) );

	int radius = ( height < width )? height / 2 : width / 2 ;

	mLightDetector->setFisheyeRadius( radius );

	while ( !mCancel ) {
		// get image from camera
		image = mCapVideo->queryFrame();
		// mirror image
		cv::flip( image, image, 1 );
		//
		cv::cvtColor( image, imageGray, CV_BGR2GRAY );
		cv::cvtColor( image, image, CV_BGR2RGB );
		//
		mLightDetector->ProcessFrame( imageGray );
		mLightDetector->DrawBoundary( image );
		mLightDetector->DrawLightContours( image );

		sendLightCoords( mLightDetector->getLight( 0 ) );
		// show image
		if ( mSendImgEnabled && !image.empty() ) {
			if ( !mShowProcessing ) {
				if ( image.data ) {
					emit pushImage( image.clone() );
				}
			}
			else {
				if ( imageGray.data ) {
					// must convert to RGB
					cv::cvtColor( imageGray, imageGray, CV_GRAY2RGB );
					emit pushImage( imageGray.clone() );
				}
			}
		}

		if ( mSendBackgrImgEnabled && !image.empty() ) {
			emit pushBackgrImage( image.clone() );
		}

		msleep( 160 );
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

void QOpenCV::LightDetectionThread::setShowProcessing( bool set )
{
	mShowProcessing = set;
}

void QOpenCV::LightDetectionThread::setFishEyeCenterX( int offset )
{
	//qDebug() << "X offset = " << offset;
	mFisheyeX = offset;
	if ( mLightDetector != nullptr ) {
		mLightDetector->setFisheyeCenter( cv::Point2d( static_cast<double>( mFisheyeX ), static_cast<double>( mFisheyeY ) ) );
	}
}

void QOpenCV::LightDetectionThread::setFishEyeCenterY( int offset )
{
	mFisheyeY = offset;
	if ( mLightDetector != nullptr ) {
		mLightDetector->setFisheyeCenter( cv::Point2d( static_cast<double>( mFisheyeX ), static_cast<double>( mFisheyeY ) ) );
	}
}

void QOpenCV::LightDetectionThread::setFishEyeRadius( int radius )
{
	mFisheyeR = radius;
	if ( mLightDetector != nullptr ) {
		mLightDetector->setFisheyeRadius( mFisheyeR );
	}
}
