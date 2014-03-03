#include "OpenCV/OpenCVCore.h"

#include <opencv2/core/core.hpp>

#include "Aruco/arucothread.h"
#include "QOpenCV/FaceRecognitionThread.h"
#include "QOpenCV/FaceRecognitionWindow.h"
#include "Viewer/CameraManipulator.h"

Q_DECLARE_METATYPE(cv::Mat)


using namespace OpenCV;


OpenCV::OpenCVCore * OpenCV::OpenCVCore::mOpenCVCore;

OpenCV::OpenCVCore::OpenCVCore( QApplication* app)
{
	mOpenCVCore = this;
	mApp		= app;
	qRegisterMetaType<cv::Mat>("Mat");
}

void OpenCV::OpenCVCore::faceRecognition()
{
	FaceRecognizer *mFaceRecognizer = new FaceRecognizer();

	mThrAruco		= new ArucoModul::ArucoThread();
	mThrFaceRec		= new QOpenCV::FaceRecognitionThread( mFaceRecognizer );
	mOpencvDialog	= new QOpenCV::FaceRecognitionWindow( AppCore::Core::getInstance( mApp )->getCoreWindow(), mApp, mThrFaceRec);


	createConnection();

	mOpencvDialog->show();
	mThrFaceRec->setWindow( mOpencvDialog );
	mThrFaceRec->start();
	//mThrAruco->start();
}

void OpenCVCore::createConnection(){

	QObject::connect( mThrFaceRec,
					  SIGNAL(sendEyesCoords(float, float, float)),
					  AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
					  SLOT(setRotationHead(float, float, float)) );

}

OpenCV::OpenCVCore * OpenCV::OpenCVCore::getInstance( QApplication* app)
{
	// if OpenCV exists
	if(mOpenCVCore == NULL)
	{
		mOpenCVCore = new OpenCV::OpenCVCore(app);
	}
	return mOpenCVCore;
}
