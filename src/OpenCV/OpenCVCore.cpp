#include "OpenCV/OpenCVCore.h"

#include <opencv2/core/core.hpp>

#include "Core/Core.h"
#include "Aruco/arucothread.h"
#include "Kinect/KinectThread.h"
#include "QOpenCV/FaceRecognitionThread.h"
#include "QOpenCV/FaceRecognitionWindow.h"
#include "QOpenCV/MarkerlessTrackingThread.h"
#include "QOpenCV/OpenCVWindow.h"
#include "Viewer/CameraManipulator.h"
#include "OpenCV/CamSelectCore.h"
#include "OpenCV/CapVideo.h"
#include "OpenCV/CameraStream.h"
#include "Viewer/MouseControl.h"
#include "QOSG/ProjectiveARCore.h"
#include "QOSG/ProjectiveARViewer.h"

OpenCV::OpenCVCore* OpenCV::OpenCVCore::mOpenCVCore;

OpenCV::OpenCVCore::OpenCVCore( QApplication* app, QWidget* parent )
{
	mOpenCVCore		= this;
	mApp			= app;
	mParent			= parent;

	mArucoThrsCreated	= false;
	mKinectThrsCreated = false;
	mOpencvWindow   =  nullptr;
	mThrFaceRec		=  nullptr;
	mThrMarkerless	=  nullptr;
	mThrAruco		=  nullptr;
	mThrKinect      =  nullptr;


}
OpenCV::OpenCVCore::~OpenCVCore( void )
{
	if ( mArucoThrsCreated ) {
		if ( mThrFaceRec != NULL ) {
			mThrFaceRec->setCancel( true );
			mThrFaceRec->setSendImgEnabled( false );
		}

		if ( mThrMarkerless != NULL ) {
			mThrMarkerless->setCancel( true );
		}

		if ( mThrAruco != NULL ) {
			mThrAruco->setCancel( true );
			mThrAruco->setSendImgEnabling( false );
		}

		if ( mOpencvWindow != NULL ) {
			mOpencvWindow->disconnect();
			mOpencvWindow->deleteLater();
		}

		if ( mThrFaceRec != NULL ) {
			mThrFaceRec->wait();
			mThrFaceRec->deleteLater();
		}

		if ( mThrMarkerless != NULL ) {
			mThrMarkerless->wait();
			mThrMarkerless->deleteLater();
		}

		if ( mThrAruco != NULL ) {
			mThrAruco->wait();
			mThrAruco->deleteLater();
		}

		delete OpenCV::CamSelectCore::getInstance();
	}
}

void OpenCV::OpenCVCore::opencvInit()
{
	if ( !mArucoThrsCreated ) {
		//create Threads
		mArucoThrsCreated = true;
		mThrAruco = new ArucoModul::ArucoThread();
		mThrFaceRec	= new QOpenCV::FaceRecognitionThread();
		mThrMarkerless = new QOpenCV::MarkerlessTrackingThread();

#ifdef OPENNI2_FOUND
		mThrKinect = new Kinect::KinectThread();
		mThrKinect->inicializeKinect();
#endif

		createPermanentConnection();
	}

	if ( !mOpencvWindow ) {
		qDebug() << "Creating OpenCV Window";
		mOpencvWindow =  new QOpenCV::OpenCVWindow( mParent, mApp );
	}

	createConnectionFaceRec();
	createConnectionMarkerless();
	createConnectionAruco();

#ifdef OPENNI2_FOUND
	createConnectionKinect();
#endif

	mOpencvWindow->show();
}

void  OpenCV::OpenCVCore::createPermanentConnection()
{
	// they are still created

	//  sending result data from face detection
	QObject::connect( mThrFaceRec,
					  SIGNAL( sendEyesCoords( float,float,float ) ),
					  AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
					  SLOT( setRotationHeadFaceDet( float,float,float ) ) );
	QOSG::CoreWindow* coreWindow = AppCore::Core::getInstance( mApp )->getCoreWindow();
	QOSG::ProjectiveARCore* projectiveARCore = QOSG::ProjectiveARCore::getInstance( mApp, coreWindow );
	//projectiveARCore->init();
	QObject::connect( mThrFaceRec,
					  SIGNAL( sendEyesRealCoords( float,float,float ) ),
					  projectiveARCore->getViewer(),
					  SLOT( setViewerPosByFaceDetection( float,float,float ) ) );

	//  sending result data from aruco - M.Garaj(TP) first ArUco try
	/*QObject::connect( mThrAruco,
					  SIGNAL( sendArucoPosVec( osg::Vec3d ) ),
					  AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
					  SLOT( updateArucoGraphPosition( osg::Vec3d ) ) );
	QObject::connect( mThrAruco,
					  SIGNAL( sendArucoRorQuat( osg::Quat ) ),
					  AppCore::Core::getInstance( mApp )->getCoreGraph(),
					  SLOT( updateGraphRotByAruco( osg::Quat ) ) );
	QObject::connect( mThrAruco,
					  SIGNAL( sendArucoPosAndQuat( osg::Quat,osg::Vec3d ) ),
					  AppCore::Core::getInstance( mApp )->getCoreGraph(),
					  SLOT( updateGraphPosAndRotByAruco( osg::Quat, osg::Vec3d ) ) );*/

	//jurik
	//sending matrices via Qt
	QObject::connect( mThrAruco,
					  SIGNAL( sendProjectionMatrix( QMatrix4x4 ) ),
					  AppCore::Core::getInstance( mApp )->getCoreGraph(),
					  SLOT( recievedPMatrix( QMatrix4x4 ) ) );
	QObject::connect( mThrAruco,
					  SIGNAL( sendModelViewMatrix( QMatrix4x4 ) ),
					  AppCore::Core::getInstance( mApp )->getCoreGraph(),
					  SLOT( recievedMVMatrix( QMatrix4x4 ) ) );

	// updating background image
	QObject::connect( mThrFaceRec,
					  SIGNAL( pushBackgrImage( cv::Mat ) ),
					  AppCore::Core::getInstance( mApp )->getCoreGraph()->getCameraStream(),
					  SLOT( updateBackgroundImage( cv::Mat ) ) );
	QObject::connect( mThrAruco,
					  SIGNAL( pushBackgrImage( cv::Mat ) ),
					  AppCore::Core::getInstance( mApp )->getCoreGraph()->getCameraStream(),
					  SLOT( updateBackgroundImage( cv::Mat ) ) );

	QObject::connect( mThrAruco,
					  SIGNAL( moveMouseArucoSignal( double,double,bool,Qt::MouseButton ) ),
					  AppCore::Core::getInstance()->getCoreWindow(),
					  SLOT( moveMouseAruco( double,double,bool,Qt::MouseButton ) ) );

}

void OpenCV::OpenCVCore::createConnectionKinect()
{

	//for video sending
	QObject::connect( mOpencvWindow,
					  SIGNAL( sendImageKinect( bool ) ),
					  mThrKinect,
					  SLOT( setImageSend( bool ) ) );

	//send image to label
	QObject::connect( mThrKinect,
					  SIGNAL( pushImage( cv::Mat ) ),
					  mOpencvWindow,
					  SLOT( setLabel( cv::Mat ) ) );

	//start
	QObject::connect( mOpencvWindow,
					  SIGNAL( startKinect() ),
					  mThrKinect,
					  SLOT( start() ) );

	//stop
	QObject::connect( mOpencvWindow,
					  SIGNAL( stopKinect( bool ) ),
					  mThrKinect,
					  SLOT( setCancel( bool ) ) );

	// moving camera with gesture
	QObject::connect( mThrKinect,
					  SIGNAL( sendSliderCoords( float,float,float ) ),
					  AppCore::Core::getInstance( NULL )->getCoreWindow()->getCameraManipulator(),
					  SLOT( setRotationHeadKinect( float,float,float ) ) );

	//enable/disable cursor movement
	QObject::connect( mOpencvWindow,
					  SIGNAL( setMovementCursor( bool ) ),
					  mThrKinect,
					  SLOT( setCursorMovement( bool ) ) );

	//enable/disable zoom
	QObject::connect( mOpencvWindow,
					  SIGNAL( setZoom( bool ) ),
					  mThrKinect,
					  SLOT( setZoomUpdate( bool ) ) );

	//edit for speed movement
	QObject::connect( mOpencvWindow,
					  SIGNAL( sendSpeedKinect( double ) ),
					  mThrKinect,
					  SLOT( setSpeedKinect( double ) ) );

	//edit for speed movement
	QObject::connect( mOpencvWindow,
					  SIGNAL( inicializeKinect() ),
					  mThrKinect,
					  SLOT( inicializeKinect() ) );

	//edit for speed movement
	QObject::connect( mOpencvWindow,
					  SIGNAL( closeActionOpenni() ),
					  mThrKinect,
					  SLOT( closeActionOpenni() ) );

	QObject::connect( mOpencvWindow,
					  SIGNAL( setKinectMarkerDetection( bool ) ),
					  mThrKinect,
					  SLOT( setImageSendToMarkerDetection( bool ) ) );

	//enable/disable sending picture to Marker Detection
	QObject::connect( mThrKinect,
					  SIGNAL( pushImageToMarkerDetection( cv::Mat ) ),
					  mThrAruco,
					  SLOT( detectMarkerFromImage( cv::Mat ) ) );

	//send augmented Image created in Kinect
	QObject::connect( mThrAruco,
					  SIGNAL( pushImageFromKinect( cv::Mat ) ),
					  mOpencvWindow,
					  SLOT( setLabel( cv::Mat ) ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( setKinectCaptureImage( bool ) ),
					  mThrKinect,
					  SLOT( setCaptureImage( bool ) ) );
}

void OpenCV::OpenCVCore::createConnectionFaceRec()
{
	// send actual image
	QObject::connect( mOpencvWindow,
					  SIGNAL( sendImgFaceRec( bool ) ),
					  mThrFaceRec,
					  SLOT( setSendImgEnabled( bool ) ) );
	QObject::connect( mThrFaceRec,
					  SIGNAL( pushImage( cv::Mat ) ),
					  mOpencvWindow,
					  SLOT( setLabel( cv::Mat ) ) );
	// send actual image to background
	QObject::connect( mOpencvWindow,
					  SIGNAL( sendBackgrImgFaceRec( bool ) ),
					  mThrFaceRec,
					  SLOT( setSendBackgrImgEnabled( bool ) ) );


	// start, stop
	QObject::connect( mOpencvWindow,
					  SIGNAL( startFaceRec() ),
					  mThrFaceRec,
					  SLOT( start() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( stopFaceRec( bool ) ),
					  mThrFaceRec,
					  SLOT( setCancel( bool ) ) );
	QObject::connect( mThrFaceRec,
					  SIGNAL( finished() ),
					  mOpencvWindow,
					  SLOT( onFaceRecThrFinished() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( setCapVideoFaceRec( OpenCV::CapVideo* ) ),
					  mThrFaceRec,
					  SLOT( setCapVideo( OpenCV::CapVideo* ) ) );
}

void OpenCV::OpenCVCore::createConnectionMarkerless()
{
	QObject::connect( mOpencvWindow,
					  SIGNAL( startMarkerless() ),
					  mThrMarkerless,
					  SLOT( start() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( stopMarkerless( bool ) ),
					  mThrMarkerless,
					  SLOT( setCancel( bool ) ) );
	QObject::connect( mThrMarkerless,
					  SIGNAL( finished() ),
					  mOpencvWindow,
					  SLOT( onMarkerlessThreadFinished() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( setCapVideoMarkerless( OpenCV::CapVideo* ) ),
					  mThrMarkerless,
					  SLOT( setCapVideo( OpenCV::CapVideo* ) ) );
	QObject::connect( mThrMarkerless,
					  SIGNAL( pushImage( cv::Mat ) ),
					  mOpencvWindow,
					  SLOT( setLabel( cv::Mat ) ) );
}

void OpenCV::OpenCVCore::createConnectionAruco()
{
	// send actual image
	QObject::connect( mOpencvWindow,
					  SIGNAL( sendImgMarker( bool ) ),
					  mThrAruco,
					  SLOT( setSendImgEnabling( bool ) ) );
	/*QObject::connect( mThrAruco,
					  SIGNAL(pushImage(QImage)),
					  mOpencvDialog,
					  SLOT(setLabelQ(QImage)) );*/
	QObject::connect( mThrAruco,
					  SIGNAL( pushImagemMat( cv::Mat ) ),
					  mOpencvWindow,
					  SLOT( setLabel( cv::Mat ) ) );


	// send actual image to background
	QObject::connect( mOpencvWindow,
					  SIGNAL( sendBackgrImgMarker( bool ) ),
					  mThrAruco,
					  SLOT( setSendBackgrImgEnabled( bool ) ) );

	// start, stop
	QObject::connect( mOpencvWindow,
					  SIGNAL( startMarker() ),
					  mThrAruco,
					  SLOT( start() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( stopMarker( bool ) ),
					  mThrAruco,
					  SLOT( setCancel( bool ) ) );
	QObject::connect( mThrAruco,
					  SIGNAL( finished() ),
					  mOpencvWindow,
					  SLOT( onMarkerThrFinished() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( setCapVideoMarker( OpenCV::CapVideo* ) ),
					  mThrAruco,
					  SLOT( setCapVideo( OpenCV::CapVideo* ) ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( startMultiMarker() ),
					  mThrAruco,
					  SLOT( start() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( stopMultiMarker( bool ) ),
					  mThrAruco,
					  SLOT( setCancel( bool ) ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( startMarker() ),
					  AppCore::Core::getInstance( mApp )->getCoreWindow(),
					  SLOT( swapManipulator() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( arucoRunning( bool ) ),
					  AppCore::Core::getInstance( mApp )->getCoreGraph(),
					  SLOT( setArucoRunning( bool ) ) );

	// other seting
	QObject::connect( mOpencvWindow->getMarkerBehindCB(),
					  SIGNAL( clicked( bool ) ),
					  mThrAruco,
					  SLOT( setPositionOfMarker( bool ) ) );
	QObject::connect( mOpencvWindow->getCorEnabledCB(),
					  SIGNAL( clicked( bool ) ),
					  mThrAruco,
					  SLOT( setCorEnabling( bool ) ) );
	QObject::connect( mOpencvWindow->getMultiMarkerEnableCB(),
					  SIGNAL( clicked( bool ) ),
					  mThrAruco,
					  SLOT( setMultiMarker( bool ) ) );
	QObject::connect( mOpencvWindow->getUpdateCorParPB(),
					  SIGNAL( clicked() ),
					  mThrAruco,
					  SLOT( updateCorectionPar() ) );
	QObject::connect( mThrAruco,
					  SIGNAL( corParUpdated() ),
					  mOpencvWindow,
					  SLOT( onCorParUpdated() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( setMultiMarker( bool ) ),
					  mThrAruco,
					  SLOT( setMultiMarker( bool ) ) );

	// aruco mouse Controll
	QObject::connect( mOpencvWindow->getInterchangeMarkersPB(),
					  SIGNAL( clicked() ),
					  mThrAruco,
					  SLOT( interchangeMarkers() ) );

}

void OpenCV::OpenCVCore::createConnectionMultiAruco()
{
	QObject::connect( mOpencvWindow,
					  SIGNAL( startMultiMarker() ),
					  mThrAruco,
					  SLOT( start() ) );
	QObject::connect( mOpencvWindow,
					  SIGNAL( setMultiMarker( bool ) ),
					  mThrAruco,
					  SLOT( setMultiMarker( bool ) ) );

	QObject::connect( mOpencvWindow,
					  SIGNAL( setCapVideoMarker( OpenCV::CapVideo* ) ),
					  mThrAruco,
					  SLOT( setCapVideo( OpenCV::CapVideo* ) ) );

	QObject::connect( mThrAruco,
					  SIGNAL( pushImagemMat( cv::Mat ) ),
					  mOpencvWindow,
					  SLOT( setLabel( cv::Mat ) ) );
}

OpenCV::OpenCVCore* OpenCV::OpenCVCore::getInstance( QApplication* app, QWidget* parent )
{
	// if OpenCV exists
	if ( mOpenCVCore == NULL ) {
		mOpenCVCore = new OpenCV::OpenCVCore( app, parent );
	}
	return mOpenCVCore;
}
