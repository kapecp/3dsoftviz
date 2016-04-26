#include "QOSG/ProjectiveARCore.h"

#include <opencv2/core/core.hpp>

#include "Core/Core.h"
#include "QOpenCV/FaceRecognitionThread.h"
#include "QOpenCV/FaceRecognitionWindow.h"
#include "QOSG/ProjectiveARWindow.h"
#include "Viewer/CameraManipulator.h"
#include "OpenCV/CamSelectCore.h"
#include "OpenCV/CapVideo.h"
#include "OpenCV/CameraStream.h"
#include "Viewer/MouseControl.h"

QOSG::ProjectiveARCore* QOSG::ProjectiveARCore::mProjectiveARCore;

QOSG::ProjectiveARCore::ProjectiveARCore( QApplication* app, QWidget* parent )
{
    mProjectiveARCore		= this;
    mApp                    = app;
    mParent                 = parent;

    mProjectiveARWindow   = NULL;
    //mThrFaceRec		= NULL;


}
QOSG::ProjectiveARCore::~ProjectiveARCore( void )
{
    /*if ( mArucoThrsCreated ) {
		if ( mThrFaceRec != NULL ) {
			mThrFaceRec->setCancel( true );
			mThrFaceRec->setSendImgEnabled( false );
		}

		if ( mOpencvWindow != NULL ) {
			mOpencvWindow->disconnect();
			mOpencvWindow->deleteLater();
		}

		if ( mThrFaceRec != NULL ) {
			mThrFaceRec->wait();
            mThrFaceRec->deleteLater();
        }

		delete OpenCV::CamSelectCore::getInstance();
    }*/
}

void QOSG::ProjectiveARCore::init(ViewerQT* sourceViewer)
{
    /*if ( !mArucoThrsCreated ) {
		//create Threads
		mArucoThrsCreated = true;
		mThrAruco = new ArucoModul::ArucoThread();
		mThrFaceRec	= new QOpenCV::FaceRecognitionThread();

		createPermanentConnection();
    }*/

    if ( !mProjectiveARWindow ) {
        qDebug() << "Creating ProjectiveAR Window";
        mProjectiveARWindow =  new QOSG::ProjectiveARWindow( mParent, sourceViewer, mApp );
	}

    //createConnectionFaceRec();
    //createConnectionAruco();

    mProjectiveARWindow->show();
}

/*void  QOSG::ProjectiveARCore::createPermanentConnection()
{
    // they are still created

	//  sending result data from face detection
	QObject::connect( mThrFaceRec,
					  SIGNAL( sendEyesCoords( float,float,float ) ),
					  AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
					  SLOT( setRotationHeadFaceDet( float,float,float ) ) );

	//  sending result data from aruco
	QObject::connect( mThrAruco,
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
					  SLOT( updateGraphPosAndRotByAruco( osg::Quat, osg::Vec3d ) ) );


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

}*/

/*void QOSG::OpenCVCore::createConnectionFaceRec()
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




}*/

QOSG::ProjectiveARCore* QOSG::ProjectiveARCore::getInstance( QApplication* app, QWidget* parent )
{
	// if OpenCV exists
    if ( mProjectiveARCore == NULL ) {
        mProjectiveARCore = new QOSG::ProjectiveARCore( app, parent );
	}
    return mProjectiveARCore;
}
