#include "OpenCV/OpenCVCore.h"

#include <opencv2/core/core.hpp>

#include "Core/Core.h"
#include "Aruco/arucothread.h"
#include "QOpenCV/FaceRecognitionThread.h"
#include "QOpenCV/FaceRecognitionWindow.h"
#include "Viewer/CameraManipulator.h"
#include "OpenCV/CamSelectCore.h"
#include "OpenCV/CapVideo.h"
#include "OpenCV/CameraStream.h"

Q_DECLARE_METATYPE(cv::Mat)


using namespace OpenCV;


OpenCV::OpenCVCore * OpenCV::OpenCVCore::mOpenCVCore;

OpenCV::OpenCVCore::OpenCVCore( QApplication* app, QWidget *parent)
{
	mOpenCVCore		= this;
	mApp			= app;
	mParent			= parent;

	mThrsCreated	= false;
	mOpencvDialog	= NULL;
	mThrFaceRec		= NULL;
	mThrAruco		= NULL;

	qRegisterMetaType<cv::Mat>("Mat");
}
OpenCV::OpenCVCore::~OpenCVCore(void)
{
	if( mThrsCreated ) {

		mThrFaceRec->setCancel(true);
		mThrAruco->setCancel(true);
		mThrFaceRec->setSendImgEnabled(false);
		mThrAruco->setSendImgEnabling(false);

		mOpencvDialog->disconnect();
		mOpencvDialog->deleteLater();

		mThrFaceRec->wait();
		mThrAruco->wait();

		mThrFaceRec->deleteLater();
		mThrAruco->deleteLater();

		delete OpenCV::CamSelectCore::getInstance();
	}
}

void OpenCV::OpenCVCore::faceRecognition()
{
	if( !mOpencvDialog ){
		if( !mThrsCreated ){
			// create threads
			qDebug() << "creating threads";
			mThrsCreated = true;
			mThrAruco		= new ArucoModul::ArucoThread();
			mThrFaceRec		= new QOpenCV::FaceRecognitionThread();
			createPermanentConnection();
		}
		// create window
		qDebug() << "creating windows";
		mOpencvDialog = new QOpenCV::FaceRecognitionWindow(
					mParent, mApp );
	}
	// if window was hidden, there no connection to threads
	if( mOpencvDialog->isHidden() ){
		createConnectionFaceRec();
		createConnectionAruco();
	}
	mOpencvDialog->show();
}

void  OpenCVCore::createPermanentConnection(){
	//  sending result data from face detection
	QObject::connect( mThrFaceRec,
					  SIGNAL(sendEyesCoords(float,float,float)),
					  AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
					  SLOT(setRotationHead(float,float,float)) );

	//  sending result data from aruco
	QObject::connect( mThrAruco,
					  SIGNAL(sendArucoPosVec(osg::Vec3d)),
					  AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
					  SLOT(updateArucoGraphPosition(osg::Vec3d)) );
	QObject::connect( mThrAruco,
					  SIGNAL(sendArucoRorQuat(osg::Quat)),
					  AppCore::Core::getInstance( mApp )->getCoreGraph(),
					  SLOT(updateGraphRotByAruco(osg::Quat)) );

	QObject::connect( mThrFaceRec,
					  SIGNAL(sendEyesCoords(float,float,float)),
					  AppCore::Core::getInstance( mApp)->getCoreWindow()->getCameraManipulator(),
					  SLOT(setRotationHead(float,float,float)) );

	// updating background image
	QObject::connect( mThrFaceRec,
					  SIGNAL(pushBackgrImage(cv::Mat)),
					  AppCore::Core::getInstance( mApp)->getCoreGraph()->getCameraStream(),
					  SLOT(updateBackgroundImage(cv::Mat)) );
	QObject::connect( mThrAruco,
					  SIGNAL(pushBackgrImage(cv::Mat)),
					  AppCore::Core::getInstance( mApp)->getCoreGraph()->getCameraStream(),
					  SLOT(updateBackgroundImage(cv::Mat)) );

}

void OpenCVCore::createConnectionFaceRec(){
	// send actual image
	QObject::connect( mOpencvDialog,
					  SIGNAL(sendImgFaceRec(bool)),
					  mThrFaceRec,
					  SLOT(setSendImgEnabled(bool)) );
	QObject::connect( mThrFaceRec,
					  SIGNAL(pushImage(cv::Mat)),
					  mOpencvDialog,
					  SLOT(setLabel(cv::Mat)) );
	// send actual image to background
	QObject::connect( mOpencvDialog,
					  SIGNAL(sendBackgrImgFaceRec(bool)),
					  mThrFaceRec,
					  SLOT(setSendBackgrImgEnabled(bool)) );



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
	QObject::connect( mOpencvDialog,
					  SIGNAL(setCapVideoFaceRec( OpenCV::CapVideo *)),
					  mThrFaceRec,
					  SLOT(setCapVideo(OpenCV::CapVideo*)) );



}

void OpenCVCore::createConnectionAruco(){
	// send actual image
	QObject::connect( mOpencvDialog,
					  SIGNAL(sendImgMarker(bool)),
					  mThrAruco,
					  SLOT(setSendImgEnabling(bool)) );
	QObject::connect( mThrAruco,
					  SIGNAL(pushImage(QImage)),
					  mOpencvDialog,
					  SLOT(setLabelQ(QImage)) );
	// send actual image to background
	QObject::connect( mOpencvDialog,
					  SIGNAL(sendBackgrImgMarker(bool)),
					  mThrAruco,
					  SLOT(setSendBackgrImgEnabled(bool)) );

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
	QObject::connect( mOpencvDialog,
					  SIGNAL(setCapVideoMarker( OpenCV::CapVideo *)),
					  mThrAruco,
					  SLOT(setCapVideo(OpenCV::CapVideo*)) );

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

OpenCV::OpenCVCore * OpenCV::OpenCVCore::getInstance(QApplication* app, QWidget *parent)
{
	// if OpenCV exists
	if(mOpenCVCore == NULL)
	{
		mOpenCVCore = new OpenCV::OpenCVCore(app, parent);
	}
	return mOpenCVCore;
}
