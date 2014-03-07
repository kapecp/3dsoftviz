#include "QOpenCV/FaceRecognitionThread.h"
#include "QOpenCV/FaceRecognitionWindow.h"

using namespace QOpenCV;

QOpenCV::FaceRecognitionThread::FaceRecognitionThread(OpenCV::FaceRecognizer* alg, QObject *parent)
	: QThread(parent)
{
	this->mFaceRecognizer = alg;
	this->cancel=false;
	this->mSendImgEnabled	= true;
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
	qDebug() << "FaceRec run start --->";
	cancel = false;
	this->mCapVideo = new OpenCV::CapVideo(0,320,240);


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
		if( mSendImgEnabled ){
			cv::Mat im = image.clone();
			emit this->pushImage(im);
		}
		if (this->mFaceRecognizer->detected)
		{
			emit this->sendEyesCoords((float)-this->mFaceRecognizer->getEyesCoords().x,
									  (float)-this->mFaceRecognizer->getEyesCoords().y,
									  -this->mFaceRecognizer->getHeadDistance(
										  cvGetCaptureProperty(
											  this->mCapVideo->getCapture(),
											  CV_CAP_PROP_FRAME_WIDTH )));
		}
	}
	qDebug() << "FaceRec run ----> end";
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
