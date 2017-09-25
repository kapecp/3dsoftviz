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
	cv::Mat imageHSV;
	mLightDetector = new OpenCV::LightDetector();

	if ( mCapVideo == NULL ) {
		qDebug() << "[LightDetectionThread::run()]  Camera is not set";
		delete mLightDetector;
		return;
	}

	// inicialize camera for lower resolution
	mCapVideo->setCaptureProperties( 640, 480 );
	mCapVideo->setAutoExposure( 0.0 );

	// initialize Fisheye lens properties
	mCapVideoWidth = mCapVideo->getWidth();
	mCapVideoHeight = mCapVideo->getHeight();
	mFishEyeCenterX = mCapVideoWidth / 2;
	mFishEyeCenterY = mCapVideoHeight / 2;
	mFishEyeRadius = mCapVideoWidth / 2;

	qDebug() << "camera height " << mCapVideoHeight;
	qDebug() << "camera width " << mCapVideoWidth;

	// initialize LightDetector
	mLightDetector->setFisheyeCenter( cv::Point( mFishEyeCenterX, mFishEyeCenterY ) );
	mLightDetector->setFisheyeRadius( mFishEyeRadius );

	//  2% of resolution for morphology open to erase small noise
	mLightDetector->setKernelOpen( mCapVideoHeight * 2 / 100 );
	// 10% of resolution for morphology close to fill holes and merge small lights
	mLightDetector->setKernelClose( mCapVideoHeight * 15 / 100 );

	while ( !mCancel ) {
		// get image from camera
		image = mCapVideo->queryFrame();

		// mirror image
		//cv::flip( image, image, 1 );
		cv::cvtColor( image, imageGray, CV_BGR2GRAY );
		cv::cvtColor( image, image, CV_BGR2RGB );
		mLightDetector->ProcessFrame( image, imageGray );

		// send lights
		for ( int i = 0; i < mLightDetector->getLightNum(); i++ ) {
			sendLightCoords( mLightDetector->getLight( i ) );
		}

		emit sendAmbientLightColor( mLightDetector->getAverageColor() * mLightDetector->getAverageColorIntensity() );

		// show image
		if ( mSendImgEnabled && !image.empty() ) {
			// camera
			if ( !mShowProcessing ) {
				if ( image.data ) {
					mLightDetector->DrawBoundary( image );
					mLightDetector->DrawLightContours( image );
					mLightDetector->DrawLightCircles( image );
					emit pushImage( image.clone() );
				}
			}
			// processing
			else {
				if ( imageGray.data ) {
					// must convert to RGB
					cv::cvtColor( imageGray, imageGray, CV_GRAY2RGB );
					emit pushImage( imageGray.clone() );
				}
			}
		}

		/*if ( mSendBackgrImgEnabled && !image.empty() ) {
			emit pushBackgrImage( image.clone() );
		}*/

		//msleep( 20 );
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
	mFishEyeCenterX = mCapVideoWidth * offset / 100;
	if ( mLightDetector != nullptr ) {
		mLightDetector->setFisheyeCenter( cv::Point( mFishEyeCenterX, mFishEyeCenterY ) );
	}
}

void QOpenCV::LightDetectionThread::setFishEyeCenterY( int offset )
{
	mFishEyeCenterY = mCapVideoHeight * offset / 100;
	if ( mLightDetector != nullptr ) {
		mLightDetector->setFisheyeCenter( cv::Point( mFishEyeCenterX, mFishEyeCenterY ) );
	}
}

void QOpenCV::LightDetectionThread::setFishEyeRadius( int radius )
{
	mFishEyeRadius = mCapVideoWidth * radius / 100;
	if ( mLightDetector != nullptr ) {
		mLightDetector->setFisheyeRadius( mFishEyeRadius );
	}
}

void QOpenCV::LightDetectionThread::setFishEyeAngle( int angle )
{
	mFishEyeAngle = angle;
	if ( mLightDetector != nullptr ) {
		mLightDetector->setFisheyeAngle( mFishEyeAngle );
	}
}
