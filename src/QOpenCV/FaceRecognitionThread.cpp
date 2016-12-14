#include "QOpenCV/FaceRecognitionThread.h"


#include "OpenCV/CapVideo.h"
#include "OpenCV/FaceRecognizer.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <QDebug>

QOpenCV::FaceRecognitionThread::FaceRecognitionThread( QObject* parent )
	: QThread( parent )
{
	mCapVideo		= NULL;
	mCancel			= false;
	mSendImgEnabled	= true;
	mSendBackgrImgEnabled = false;
}


QOpenCV::FaceRecognitionThread::~FaceRecognitionThread( void )
{
}

// in a loop, capture frame from camera and detect faces
// send eyes coordinates to change view
void QOpenCV::FaceRecognitionThread::run()
{
	mCancel = false;
	cv::Mat image;
	OpenCV::FaceRecognizer*	mFaceRecognizer = new OpenCV::FaceRecognizer();

	if ( mCapVideo == NULL ) {
		qDebug() << "[FaceRecognitionThread::run()]  Camera is not set";
		delete mFaceRecognizer;
		return;
	}

	while ( !mCancel ) {
		// get image from camera
		image = mCapVideo->queryFrame();

		cv::cvtColor( image, image, CV_BGR2RGB );

		// we detect faces on grayscale image
		mFaceRecognizer->detectFaces( mCapVideo->getGrayframe() );
		mFaceRecognizer->annotateFaces( image );

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

		// when face was detected along with movement (implemented with threshold)
		// send eyes coordinate to change view
		if ( mFaceRecognizer->detected ) { //&& mFaceRecognizer->isMovement
			emit sendEyesCoords( static_cast<float>( -mFaceRecognizer->getEyesCoords().x ),
								 static_cast<float>( -mFaceRecognizer->getEyesCoords().y ),
								 -mFaceRecognizer->getHeadDistance( mCapVideo->getWidth() ) );
			// Kostan:
			//  focalLength - used my webcam focal length
			//  0.1481m - face width avg (15yr - 40yr)
			float focalLength = 749.3f;
			float distance = mFaceRecognizer->getHeadDistanceFocal( 0.1481 );
			float x = static_cast<float>( -mFaceRecognizer->getEyesCoords().x * ( mCapVideo->getWidth()/200 ) ) * ( distance / focalLength );
			float y = static_cast<float>( -mFaceRecognizer->getEyesCoords().y * ( mCapVideo->getHeight()/200 ) ) * ( distance / focalLength );
			emit sendEyesRealCoords( x, y, distance );
		}
		msleep( 80 );
	}
	mCapVideo->release();
	mCapVideo = NULL;
	delete mFaceRecognizer;
}

void QOpenCV::FaceRecognitionThread::pauseWindow()
{
	mCancel = true;
}

void QOpenCV::FaceRecognitionThread::setCancel( bool set )
{
	mCancel = set;
}

void QOpenCV::FaceRecognitionThread::setSendImgEnabled( bool sendImgEnabled )
{
	mSendImgEnabled = sendImgEnabled;
}

void QOpenCV::FaceRecognitionThread::setSendBackgrImgEnabled( bool sendBackgrImgEnabled )
{
	mSendBackgrImgEnabled = sendBackgrImgEnabled;
}



void QOpenCV::FaceRecognitionThread::setCapVideo( OpenCV::CapVideo* capVideo )
{
	mCapVideo = capVideo;
}

