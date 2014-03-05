#include "QOpenCV/FaceRecognitionThread.h"
#include "OpenCV/CamSelectCore.h"

using namespace QOpenCV;

QOpenCV::FaceRecognitionThread::FaceRecognitionThread(OpenCV::FaceRecognizer* alg)
{
	this->mFaceRecognizer = alg;
	this->mCapVideo = OpenCV::CamSelectCore::getInstance(NULL)->selectCamera();
	this->cancel=false;
	qRegisterMetaType<cv::Mat>("Mat");
}

QOpenCV::FaceRecognitionThread::~FaceRecognitionThread(void)
{
	this->mCapVideo->release();
	delete this->mFaceRecognizer;
}

void QOpenCV::FaceRecognitionThread::setWindow(FaceRecognitionWindow *mFaceRecognitionWindow)
{
	this->mFaceRecognitionWindow = mFaceRecognitionWindow;
	connect(this->mFaceRecognitionWindow,SIGNAL(cancelLoop(bool)),this,SLOT(setCancel(bool)));
}

void QOpenCV::FaceRecognitionThread::run()
{
	if (!this->mCapVideo->isOpened()){
		qDebug() << "Camera is not opened";
		return;
	}
	cv::Mat image;
	while(!cancel) {
		image=this->mCapVideo->queryFrame();
		this->mCapVideo->createGray();

		this->mFaceRecognizer->detectFaces(this->mCapVideo->getGrayframe());
		this->mFaceRecognizer->annotateFaces(image);
		cv::Mat im = image.clone();
		emit this->pushImage(im);
		if (this->mFaceRecognizer->detected && this->mFaceRecognizer->isMovement)
		{
			emit this->sendEyesCoords((float)-this->mFaceRecognizer->getEyesCoords().x,
									  (float)-this->mFaceRecognizer->getEyesCoords().y,
									  -this->mFaceRecognizer->getHeadDistance(
										 this->mCapVideo->getWidth()));
		}
	}
}

void QOpenCV::FaceRecognitionThread::pauseWindow()
{
	this->cancel=true;
}

void QOpenCV::FaceRecognitionThread::setCancel(bool set){
	this->cancel=set;
}
