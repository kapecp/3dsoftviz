#include "QOpenCV/FaceRecognitionThread.h"

#include "OpenCV/CapVideo.h"
#include "OpenCV/FaceRecognizer.h"

using namespace QOpenCV;

QOpenCV::FaceRecognitionThread::FaceRecognitionThread( QObject *parent)
	: QThread(parent)
{
	mCapVideo		= NULL;
	mCancel			= false;
	mSendImgEnabled	= true;
	mSendBackgrImgEnabled = false;
}


QOpenCV::FaceRecognitionThread::~FaceRecognitionThread(void)
{

}

void QOpenCV::FaceRecognitionThread::run()
{
	mCancel = false;
	cv::Mat image;
	OpenCV::FaceRecognizer	*mFaceRecognizer = new OpenCV::FaceRecognizer();

	if( mCapVideo == NULL){
		qDebug() << "[FaceRecognitionThread::run()]  Camera is not set";
		return;
	}

	while( !mCancel ){
		image = mCapVideo->queryFrame();

		mFaceRecognizer->detectFaces( mCapVideo->getGrayframe() );
		mFaceRecognizer->annotateFaces( image );

		if( mSendImgEnabled && !image.empty()){
			if (image.data)
				emit pushImage( image.clone() ); // ???
		}
		if( mSendBackgrImgEnabled && !image.empty()){
			emit pushBackgrImage( image.clone() );
		}


		if( mFaceRecognizer->detected ) { //&& mFaceRecognizer->isMovement
			emit sendEyesCoords( (float) -mFaceRecognizer->getEyesCoords().x,
								 (float) -mFaceRecognizer->getEyesCoords().y,
								 -mFaceRecognizer->getHeadDistance( mCapVideo->getWidth()) );
		}
		msleep(80);
	}
	mCapVideo->release();
	mCapVideo = NULL;
	delete mFaceRecognizer;
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

void QOpenCV::FaceRecognitionThread::setSendBackgrImgEnabled( bool sendBackgrImgEnabled )
{
	mSendBackgrImgEnabled = sendBackgrImgEnabled;
}



void QOpenCV::FaceRecognitionThread::setCapVideo( OpenCV::CapVideo *capVideo){
	mCapVideo = capVideo;
}

