#include "QOpenCV/FaceRecognitionThread.h"

#include "OpenCV/CapVideo.h"
#include "OpenCV/FaceRecognizer.h"

using namespace QOpenCV;

QOpenCV::FaceRecognitionThread::FaceRecognitionThread( OpenCV::FaceRecognizer *faceRecognizer, QObject *parent)
	: QThread(parent)
{
	mFaceRecognizer	= faceRecognizer;
	mCapVideo		= NULL;
	mCancel			= false;
	mSendImgEnabled	= true;
}


QOpenCV::FaceRecognitionThread::~FaceRecognitionThread(void)
{
	delete mFaceRecognizer;
}


void QOpenCV::FaceRecognitionThread::run()
{
	mCancel = false;
	cv::Mat image;

	if( mCapVideo == NULL){
		qDebug() << "[FaceRecognitionThread::run()]  Camera is not set";
		return;
	}

	while( !mCancel ){
		image = mCapVideo->queryFrame();

		mFaceRecognizer->detectFaces( mCapVideo->getGrayframe() );
		mFaceRecognizer->annotateFaces( image );

		if( mSendImgEnabled ){
			emit pushImage( image.clone() ); // ???
		}

		if( mFaceRecognizer->detected && mFaceRecognizer->isMovement ) {
			emit sendEyesCoords( (float) -mFaceRecognizer->getEyesCoords().x,
								 (float) -mFaceRecognizer->getEyesCoords().y,
								 -mFaceRecognizer->getHeadDistance( mCapVideo->getWidth()) );
		}
	}
	mCapVideo->release();
	mCapVideo = NULL;
}

void QOpenCV::FaceRecognitionThread::pauseWindow()
{
	mCancel = true;
}

void QOpenCV::FaceRecognitionThread::setCancel( bool set ){
	mCancel = set;
}

void QOpenCV::FaceRecognitionThread::setSendImgEnabled( bool sendImgEnabled )
{
	mSendImgEnabled = sendImgEnabled;
}

void QOpenCV::FaceRecognitionThread::setCapVideo( OpenCV::CapVideo *capVideo){
	mCapVideo = capVideo;
}
