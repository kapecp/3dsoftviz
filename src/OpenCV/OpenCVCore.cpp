#include "OpenCV/OpenCVCore.h"

#include <opencv2/core/core.hpp>

#include "Core/Core.h"
#include "Aruco/arucothread.h"
#include "QOpenCV/FaceRecognitionThread.h"
#include "QOpenCV/FaceRecognitionWindow.h"
#include "QOpenCV/OpenCVWindow.h"
#include "Viewer/CameraManipulator.h"
#include "OpenCV/CamSelectCore.h"
#include "OpenCV/CapVideo.h"
#include "OpenCV/CameraStream.h"
#include "Viewer/MouseControl.h"

OpenCV::OpenCVCore* OpenCV::OpenCVCore::mOpenCVCore;

OpenCV::OpenCVCore::OpenCVCore( QApplication* app, QWidget* parent )
{
    mOpenCVCore		= this;
    mApp			= app;
    mParent			= parent;

    mThrsCreated	= false;
    mOpencvDialog	= NULL;
    mOpencvWindow   = NULL;
    mThrFaceRec		= NULL;
    mThrAruco		= NULL;


}
OpenCV::OpenCVCore::~OpenCVCore( void )
{
    if( mThrsCreated ) {
        if(mThrFaceRec != NULL)
        {
            mThrFaceRec->setCancel(true);
            mThrFaceRec->setSendImgEnabled(false);
        }

        if(mThrAruco != NULL)
        {
            mThrAruco->setCancel(true);
            mThrAruco->setSendImgEnabling(false);
        }

        if(mOpencvDialog != NULL)
        {
            mOpencvDialog->disconnect();
            mOpencvDialog->deleteLater();
        }

        if(mOpencvWindow != NULL)
        {
            mOpencvWindow->disconnect();
            mOpencvWindow->deleteLater();
        }

        if(mThrFaceRec != NULL)
        {
            mThrFaceRec->wait();
            mThrFaceRec->deleteLater();
        }

        if(mThrAruco != NULL)
        {
            mThrAruco->wait();
            mThrAruco->deleteLater();
        }

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

void OpenCV::OpenCVCore::opencvInit()
{
    if( !mOpencvWindow ) {
        if( !mThrsCreated ) {
            //create Threads
            mThrsCreated = true;
            mThrAruco = new ArucoModul::ArucoThread();
            mThrFaceRec	= new QOpenCV::FaceRecognitionThread();
            createPermanentConnection();
        }
        qDebug() << "Creating OpenCV Window";
        mOpencvWindow =  new QOpenCV::OpenCVWindow(mParent, mApp);
    }

    if( mOpencvWindow->isHidden() )
    {
        //createConnectionMultiAruco();
        createConnectionFaceRec();
        createConnectionAruco();
    }

    mOpencvWindow->show();
}

void  OpenCV::OpenCVCore::createPermanentConnection(){
    // they are still created

    //  sending result data from face detection
    QObject::connect( mThrFaceRec,
                      SIGNAL(sendEyesCoords(float,float,float)),
                      AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
                      SLOT(setRotationHeadFaceDet(float,float,float)) );

    //  sending result data from aruco
    QObject::connect( mThrAruco,
                      SIGNAL(sendArucoPosVec(osg::Vec3d)),
                      AppCore::Core::getInstance( mApp )->getCoreWindow()->getCameraManipulator(),
                      SLOT(updateArucoGraphPosition(osg::Vec3d)) );
    QObject::connect( mThrAruco,
                      SIGNAL(sendArucoRorQuat(osg::Quat)),
                      AppCore::Core::getInstance( mApp )->getCoreGraph(),
                      SLOT(updateGraphRotByAruco(osg::Quat)) );


    // updating background image
    QObject::connect( mThrFaceRec,
                      SIGNAL(pushBackgrImage(cv::Mat)),
                      AppCore::Core::getInstance( mApp)->getCoreGraph()->getCameraStream(),
                      SLOT(updateBackgroundImage(cv::Mat)) );
    QObject::connect( mThrAruco,
                      SIGNAL(pushBackgrImage(cv::Mat)),
                      AppCore::Core::getInstance( mApp)->getCoreGraph()->getCameraStream(),
                      SLOT(updateBackgroundImage(cv::Mat)) );

    QObject::connect(mThrAruco,
                     SIGNAL(moveMouseArucoSignal(double,double,bool,Qt::MouseButton)),
                     AppCore::Core::getInstance()->getCoreWindow(),
                     SLOT(moveMouseAruco(double,double,bool,Qt::MouseButton)));

}

void OpenCV::OpenCVCore::createConnectionFaceRec(){
    // send actual image
    QObject::connect( mOpencvWindow,
                      SIGNAL(sendImgFaceRec(bool)),
                      mThrFaceRec,
                      SLOT(setSendImgEnabled(bool)) );
    QObject::connect( mThrFaceRec,
                      SIGNAL(pushImage(cv::Mat)),
                      mOpencvWindow,
                      SLOT(setLabel(cv::Mat)) );
    // send actual image to background
    QObject::connect( mOpencvWindow,
                      SIGNAL(sendBackgrImgFaceRec(bool)),
                      mThrFaceRec,
                      SLOT(setSendBackgrImgEnabled(bool)) );



    // start, stop
    QObject::connect( mOpencvWindow,
                      SIGNAL(startFaceRec()),
                      mThrFaceRec,
                      SLOT(start()) );
    QObject::connect( mOpencvWindow,
                      SIGNAL(stopFaceRec(bool)),
                      mThrFaceRec,
                      SLOT(setCancel(bool)) );
    QObject::connect( mThrFaceRec,
                      SIGNAL(finished()),
                      mOpencvWindow,
                      SLOT(onFaceRecThrFinished()) );
    QObject::connect( mOpencvWindow,
                      SIGNAL(setCapVideoFaceRec( OpenCV::CapVideo *)),
                      mThrFaceRec,
                      SLOT(setCapVideo(OpenCV::CapVideo*)) );




}


void OpenCV::OpenCVCore::createConnectionAruco(){
    // send actual image
    QObject::connect( mOpencvWindow,
                      SIGNAL(sendImgMarker(bool)),
                      mThrAruco,
                      SLOT(setSendImgEnabling(bool)) );
    /*QObject::connect( mThrAruco,
                      SIGNAL(pushImage(QImage)),
                      mOpencvDialog,
                      SLOT(setLabelQ(QImage)) );*/
    QObject::connect( mThrAruco,
                      SIGNAL(pushImagemMat(cv::Mat)),
                      mOpencvWindow,
                      SLOT(setLabel(cv::Mat)) );


    // send actual image to background
    QObject::connect( mOpencvWindow,
                      SIGNAL(sendBackgrImgMarker(bool)),
                      mThrAruco,
                      SLOT(setSendBackgrImgEnabled(bool)) );

    // start, stop
    QObject::connect( mOpencvWindow,
                      SIGNAL(startMarker()),
                      mThrAruco,
                      SLOT(start()) );
    QObject::connect( mOpencvWindow,
                      SIGNAL(stopMarker(bool)),
                      mThrAruco,
                      SLOT(setCancel(bool)) );
    QObject::connect( mThrAruco,
                      SIGNAL(finished()),
                      mOpencvWindow,
                      SLOT(onMarkerThrFinished()) );
    QObject::connect( mOpencvWindow,
                      SIGNAL(setCapVideoMarker( OpenCV::CapVideo *)),
                      mThrAruco,
                      SLOT(setCapVideo(OpenCV::CapVideo*)) );
    QObject::connect( mOpencvWindow,
                      SIGNAL(startMultiMarker()),
                      mThrAruco,
                      SLOT(start()));
    QObject::connect( mOpencvWindow,
                      SIGNAL(stopMultiMarker(bool)),
                      mThrAruco,
                      SLOT(setCancel(bool)) );


    // other seting
    QObject::connect( mOpencvWindow->getMarkerBehindCB(),
                      SIGNAL(clicked(bool)),
                      mThrAruco,
                      SLOT(setPositionOfMarker(bool)) );
    QObject::connect( mOpencvWindow->getCorEnabledCB(),
                      SIGNAL(clicked(bool)),
                      mThrAruco,
                      SLOT(setCorEnabling(bool)) );
    QObject::connect( mOpencvWindow->getUpdateCorParPB(),
                      SIGNAL(clicked()),
                      mThrAruco,
                      SLOT(updateCorectionPar()) );
    QObject::connect( mThrAruco,
                      SIGNAL(corParUpdated()),
                      mOpencvWindow,
                      SLOT(onCorParUpdated()) );
    QObject::connect( mOpencvWindow,
                      SIGNAL(setMultiMarker(bool)),
                      mThrAruco,
                      SLOT(setMultiMarker(bool)) );

    // aruco mouse Controll
    QObject::connect( mOpencvWindow->getInterchangeMarkersPB(),
                      SIGNAL(clicked()),
                      mThrAruco,
                      SLOT(interchangeMarkers()) );

}

void OpenCV::OpenCVCore::createConnectionMultiAruco()
{
    QObject::connect( mOpencvWindow,
                      SIGNAL(startMultiMarker()),
                      mThrAruco,
                      SLOT(start()));
    QObject::connect( mOpencvWindow,
                      SIGNAL(setMultiMarker(bool)),
                      mThrAruco,
                      SLOT(setMultiMarker(bool)) );

    QObject::connect( mOpencvWindow,
                      SIGNAL(setCapVideoMarker( OpenCV::CapVideo *)),
                      mThrAruco,
                      SLOT(setCapVideo(OpenCV::CapVideo*)) );

    QObject::connect( mThrAruco,
                      SIGNAL(pushImagemMat(cv::Mat)),
                      mOpencvWindow,
                      SLOT(setLabel(cv::Mat)) );
}

OpenCV::OpenCVCore* OpenCV::OpenCVCore::getInstance( QApplication* app, QWidget* parent )
{
    // if OpenCV exists
    if(mOpenCVCore == NULL)
    {
        mOpenCVCore = new OpenCV::OpenCVCore(app, parent);
    }
    return mOpenCVCore;
}
