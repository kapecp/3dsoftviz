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
	cv::Mat image;
	while(!cancel) {
		image=this->mCapVideo->queryFrame();
		this->mCapVideo->createGray();

		this->mFaceRecognizer->detectFaces(this->mCapVideo->getGrayframe());
		this->mFaceRecognizer->annotateFaces(image);
		cv::Mat im = image.clone();
		emit this->pushImage(im);
		if (this->mFaceRecognizer->detected)
		{
			emit this->sendEyesCoords(-this->mFaceRecognizer->getEyesCoords().x,
									  -this->mFaceRecognizer->getEyesCoords().y,
									  0.0 );
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
