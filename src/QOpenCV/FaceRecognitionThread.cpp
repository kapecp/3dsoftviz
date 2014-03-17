#include "QOpenCV/FaceRecognitionThread.h"

#include "OpenCV/CamSelectCore.h"

#include "QOpenCV/FaceRecognitionWindow.h"  // ????


using namespace QOpenCV;

QOpenCV::FaceRecognitionThread::FaceRecognitionThread(OpenCV::FaceRecognizer* alg, QObject *parent)
	: QThread(parent)
{
	this->mFaceRecognizer = alg;

	this->mCapVideo = OpenCV::CamSelectCore::getInstance()->selectCamera();
	this->cancel=false;
	this->mSendImgEnabled	= true;
	mCapVideo = NULL;

}


QOpenCV::FaceRecognitionThread::~FaceRecognitionThread(void)
{

	if(this->mCapVideo!=NULL){
		this->mCapVideo->release();
	}
	delete this->mFaceRecognizer;
}


void QOpenCV::FaceRecognitionThread::run()
{

	cancel = false;

	//if (!this->mCapVideo->getCapture()){ ???
	if (this->mCapVideo==NULL){
		qDebug() << "Camera is not opened";
		return;
	}
	cv::Mat image;
	while(!cancel) {
		image=this->mCapVideo->queryFrame();
		this->mCapVideo->createGray();

		this->mFaceRecognizer->detectFaces(this->mCapVideo->getGrayframe());
		this->mFaceRecognizer->annotateFaces(image);

		if( mSendImgEnabled ){
			cv::Mat im = image.clone();
			emit this->pushImage(im);
		}

		if (this->mFaceRecognizer->detected && this->mFaceRecognizer->isMovement) {
			emit this->sendEyesCoords((float)-this->mFaceRecognizer->getEyesCoords().x,
									  (float)-this->mFaceRecognizer->getEyesCoords().y,
									  -this->mFaceRecognizer->getHeadDistance(
										 this->mCapVideo->getWidth()));
		}
	}
	delete this->mCapVideo;
}

void QOpenCV::FaceRecognitionThread::pauseWindow()
{
	this->cancel=true;
}

void QOpenCV::FaceRecognitionThread::setCancel(bool set){
	this->cancel=set;
}

void QOpenCV::FaceRecognitionThread::setSendImgEnabled( bool sendImgEnabled )
{
	mSendImgEnabled = sendImgEnabled;
}
