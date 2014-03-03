#include "OpenCV/OpenCVCore.h"
#include "Viewer/CameraManipulator.h"
#include "OpenCV/CamSelectCore.h"

using namespace OpenCV;


OpenCV::OpenCVCore * OpenCV::OpenCVCore::mOpenCVCore;

OpenCV::OpenCVCore::OpenCVCore( QApplication* app)
{
	mOpenCVCore = this;
	this->app=app;
}

void OpenCV::OpenCVCore::faceRecognition()
{
	OpenCVCam::CamSelectCore::getInstance(this->app)->selectCamera();
	//	OpenCV::FaceRecognizer *mFaceRecognizer = new OpenCV::FaceRecognizer();

	//	QOpenCV::FaceRecognitionThread *thr = new QOpenCV::FaceRecognitionThread(mFaceRecognizer);
	//	QOpenCV::FaceRecognitionWindow *cvw = new QOpenCV::FaceRecognitionWindow(AppCore::Core::getInstance(this->app)->getCoreWindow(),this->app,thr);



	//	QObject::connect( thr,SIGNAL(sendEyesCoords(float,float,float)),
	//					  AppCore::Core::getInstance(this->app)->getCoreWindow()->getCameraManipulator(),
	//					  SLOT(setRotationHead(float,float,float)) );

	//	cvw->show();
	//	thr->setWindow(cvw);
	//	thr->start();
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
