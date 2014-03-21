#include "QOpenCV/FaceRecognitionThread.h"

#include "OpenCV/CamSelectCore.h"

#include "QOpenCV/FaceRecognitionWindow.h"  // ????


using namespace QOpenCV;

QOpenCV::FaceRecognitionThread::FaceRecognitionThread( OpenCV::FaceRecognizer *faceRecognizer, QObject *parent)
	: QThread(parent)
{
	mFaceRecognizer	= faceRecognizer;
	mCapVideo	= NULL;
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

	mCapVideo = OpenCV::CamSelectCore::getInstance()->selectCamera();
	if( mCapVideo == NULL){
		qDebug() << "Camera is not opened";
		return;
	}

	while( !mCancel ){
		image = mCapVideo->queryFrame();
		mCapVideo->createGray();

		mFaceRecognizer->detectFaces( mCapVideo->getGrayframe() );
		mFaceRecognizer->annotateFaces( image );

		if( mSendImgEnabled ){
			//cv::Mat im = ; // ???
			emit pushImage( image.clone() ); // ???
		}

		if( mFaceRecognizer->detected && mFaceRecognizer->isMovement ) {
			emit sendEyesCoords( (float) -mFaceRecognizer->getEyesCoords().x,
								 (float) -mFaceRecognizer->getEyesCoords().y,
								 -mFaceRecognizer->getHeadDistance( mCapVideo->getWidth()) );
		}
	}
	mCapVideo->release();
	//delete this->mCapVideo; ???
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
