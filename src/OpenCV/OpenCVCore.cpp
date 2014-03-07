#include "OpenCV/OpenCVCore.h"

#include <opencv2/core/core.hpp>

#include "Core/Core.h"
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
	mOpencvDialog	= new QOpenCV::FaceRecognitionWindow( AppCore::Core::getInstance( mApp )->getCoreWindow(), mApp, mThrFaceRec, mThrAruco);


	createConnectionFaceRec();
	createConnectionAruco();

	mOpencvDialog->show();
	mThrFaceRec->setWindow( mOpencvDialog );
}

void OpenCVCore::createConnectionFaceRec(){

	//  sending result data
	QObject::connect( mThrFaceRec,
					  SIGNAL(sendEyesCoords(float,float,float)),
					  AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
					  SLOT(setRotationHead(float,float,float)) );



	// send actual image
	QObject::connect( mOpencvDialog,
					  SIGNAL(sendImgFaceRec(bool)),
					  mThrFaceRec,
					  SLOT(setSendImgEnabled(bool)) );
	QObject::connect( mThrFaceRec,
					  SIGNAL(pushImage(cv::Mat)),
					  mOpencvDialog,
					  SLOT(setLabel(cv::Mat)) );

	// start, stop
	QObject::connect( mOpencvDialog,
					  SIGNAL(startFaceRec()),
					  mThrFaceRec,
					  SLOT(start()) );
	QObject::connect( mOpencvDialog,
					  SIGNAL(stopFaceRec(bool)),
					  mThrFaceRec,
					  SLOT(setCancel(bool)) );
	QObject::connect( mThrFaceRec,
					  SIGNAL(finished()),
					  mOpencvDialog,
					  SLOT(onFaceRecThrFinished()) );

}

void OpenCVCore::createConnectionAruco(){

	//  sending result data
	QObject::connect( mThrAruco,
					  SIGNAL(sendArucoPosVec(osg::Vec3d)),
					  AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
					  SLOT(updateArucoGraphPosition(osg::Vec3d)) );
	QObject::connect( mThrAruco,
					  SIGNAL(sendArucoRorQuat(osg::Quat)),
					  AppCore::Core::getInstance( mApp )->getCoreGraph(),
					  SLOT(updateArucoGraphRotation(osg::Quat)) );

	// send actual image
	QObject::connect( mOpencvDialog,
					  SIGNAL(sendImgMarker(bool)),
					  mThrAruco,
					  SLOT(setSendImgEnabling(bool)) );
	QObject::connect( mThrAruco,
					  SIGNAL(pushImage(cv::Mat)),
					  mOpencvDialog,
					  SLOT(setLabel(cv::Mat)) );

	// start, stop
	QObject::connect( mOpencvDialog,
					  SIGNAL(startMarker()),
					  mThrAruco,
					  SLOT(start()) );
	QObject::connect( mOpencvDialog,
					  SIGNAL(stopMarker(bool)),
					  mThrAruco,
					  SLOT(setCancel(bool)) );
	QObject::connect( mThrAruco,
					  SIGNAL(finished()),
					  mOpencvDialog,
					  SLOT(onMarkerThrFinished()) );

	// other seting
	QObject::connect( mOpencvDialog->getMarkerBehindCB(),
					  SIGNAL(clicked(bool)),
					  mThrAruco,
					  SLOT(setPositionOfMarker(bool)) );
	QObject::connect( mOpencvDialog->getCorEnabledCB(),
					  SIGNAL(clicked(bool)),
					  mThrAruco,
					  SLOT(setCorEnabling(bool)) );
	QObject::connect( mOpencvDialog->getUpdateCorParPB(),
					  SIGNAL(clicked()),
					  mThrAruco,
					  SLOT(updateCorectionPar()) );
	QObject::connect( mThrAruco,
					  SIGNAL(corParUpdated()),
					  mOpencvDialog,
					  SLOT(onCorParUpdated()) );

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
