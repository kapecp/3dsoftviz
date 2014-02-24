#include "QOpenCV/FaceRecognitionThread.h"

using namespace QOpenCV;

QOpenCV::FaceRecognitionThread::FaceRecognitionThread(OpenCV::FaceRecognizer* alg)
{
	this->mFaceRecognizer = alg;
	this->mCapVideo = new OpenCV::CapVideo(0,320,240);
	this->cancel=false;
	qRegisterMetaType<cv::Mat>("Mat");
}

QOpenCV::FaceRecognitionThread::~FaceRecognitionThread(void)
{
	delete this->mCapVideo;
	delete this->mFaceRecognizer;
}

void QOpenCV::FaceRecognitionThread::setWindow(FaceRecognitionWindow *mFaceRecognitionWindow)
{
	this->mFaceRecognitionWindow = mFaceRecognitionWindow;
	connect(this->mFaceRecognitionWindow,SIGNAL(cancelLoop(bool)),this,SLOT(setCancel(bool)));
}

void QOpenCV::FaceRecognitionThread::run()
{
	if (!this->mCapVideo->getCapture()){
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
			emit this->sendEyesCoords(-this->mFaceRecognizer->getEyesCoords().x,
									  -this->mFaceRecognizer->getEyesCoords().y,
									  -this->mFaceRecognizer->getHeadDistance(
										  cvGetCaptureProperty(
											  this->mCapVideo->getCapture(),
											  CV_CAP_PROP_FRAME_WIDTH )));
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
