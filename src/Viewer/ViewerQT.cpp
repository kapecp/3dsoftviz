#include "QOSG/ViewerQT.h"

#include "Viewer/PickHandler.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/CoreGraph.h"
#include "Util/ApplicationConfig.h"
#include "Core/Core.h"

QOSG::ViewerQT::ViewerQT( QWidget* parent , const char* name , const QGLWidget* shareWidget , WindowFlags f , Vwr::CoreGraph* cg ):
	AdapterWidget( parent, name, shareWidget, f )
{
	this->cg = cg;
	cg->setCamera( this->getCamera() );

	this->mIsClicAruco=false;

	appConf = Util::ApplicationConfig::get();

	osg::DisplaySettings::instance()->setStereo( ( appConf->getValue( "Viewer.Display.Stereoscopic" ).toInt() ? true : false ) );
	osg::DisplaySettings::instance()->setStereoMode( osg::DisplaySettings::ANAGLYPHIC );

	getCamera()->setViewport( new osg::Viewport( 0,0,width(),height() ) );
	getCamera()->setProjectionMatrixAsPerspective( 60, static_cast<double>( width() )/static_cast<double>( height() ), 0.01, appConf->getValue( "Viewer.Display.ViewDistance" ).toFloat() );
	getCamera()->setGraphicsContext( getGraphicsWindow() );
	getCamera()->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );

	manipulator = new Vwr::CameraManipulator( cg );
	QObject::connect( manipulator, SIGNAL( sendMouseRotation( osg::Quat ) ),
					  cg, SLOT( updateGraphRotByMouse( osg::Quat ) ) );
	QObject::connect( manipulator, SIGNAL( sendFaceDetRotation( osg::Quat ) ),
					  cg, SLOT( updateGraphRotByFaceDet( osg::Quat ) ) );

	pickHandler = new Vwr::PickHandler( manipulator, cg );

	addEventHandler( new osgViewer::StatsHandler );
	addEventHandler( pickHandler );
	setCameraManipulator( manipulator );

	// Gloger: background to white color after disabling skybox
//	getCamera()->setClearColor( osg::Vec4( 0, 0, 0, 1 ) );
//    getCamera()->setClearColor( osg::Vec4( 0.85, 0.85, 0.85, 1 ) );
    getCamera()->setClearColor( osg::Vec4( 1, 1, 1, 1 ) );
	getCamera()->setViewMatrixAsLookAt( osg::Vec3d( -10, 0, 0 ), osg::Vec3d( 0, 0, 0 ), osg::Vec3d( 0, 1, 0 ) );

	setThreadingModel( osgViewer::ViewerBase::SingleThreaded );

	connect( &_timer, SIGNAL( timeout() ), this, SLOT( updateGL() ) );

	_timer.start( 10 );
}

Vwr::PickHandler* QOSG::ViewerQT::getPickHandler() const
{
	return pickHandler;
}

Vwr::CameraManipulator* QOSG::ViewerQT::getCameraManipulator() const
{
	return manipulator;
}

void QOSG::ViewerQT::reloadConfig()
{
	manipulator->setMaxSpeed( appConf->getValue( "Viewer.CameraManipulator.MaxSpeed" ).toFloat() );
	manipulator->setTrackballSize( appConf->getValue( "Viewer.CameraManipulator.Sensitivity" ).toFloat() );

	osg::DisplaySettings::instance()->setStereo( ( appConf->getValue( "Viewer.Display.Stereoscopic" ).toInt() ? true : false ) );
}

void QOSG::ViewerQT::paintGL()
{
	frame();
	cg->update();
}

void QOSG::ViewerQT::moveMouseAruco( double positionX,double positionY,bool isClick,int windowX,int windowY ,Qt::MouseButton button )
{
	//qDebug() << positionX << "  " << positionY << "         " << isClick;

	float wieverX = static_cast<float>( positionX * static_cast<float>( this->width() ) );
	float wieverY = static_cast<float>( positionY * static_cast<float>( this->height() ) );

	int screenX = static_cast<int>( positionX * this->width()  + this->x() + windowX + 8 );
	int screenY = static_cast<int>( positionY * this->height() + this->y() + windowY + 28 );

	this->cursor().setPos( screenX, screenY );

	qDebug() << screenX << "  " << screenY ;
	wieverY = ( static_cast<float>( this->height() ) - wieverY );

	if ( isClick != mIsClicAruco ) {
		mIsClicAruco = isClick;

		if ( isClick ) {
			this->getEventQueue()->mouseButtonPress( wieverX, wieverY,button );
			this->getEventQueue()->mouseMotion( wieverX, wieverY );
		}
		else {
			this->getEventQueue()->mouseButtonRelease( wieverX, wieverY, button );
			return;
		}
	}
	this->getEventQueue()->mouseMotion( wieverX, wieverY );
}

void QOSG::ViewerQT::moveMouseKinect( double positionX,double positionY,double speed,bool isClick,int windowX,int windowY ,Qt::MouseButton button )
{
	//qDebug() << positionX << "  " << positionY << "         " << isClick;
	positionX /=640.0;
	positionY/=480.0;

	float wieverX = static_cast<float>( positionX * static_cast<float>( this->width() ) );
	float wieverY = static_cast<float>( positionY * static_cast<float>( this->height() ) );

	int screenX = static_cast<int>( positionX * this->width()  + this->x() + windowX + 8 );
	int screenY = static_cast<int>( positionY * this->height() + this->y() + windowY + 28 );

	this->cursor().setPos( screenX, screenY );

	qDebug() << screenX << "  " << screenY ;
	wieverY = ( static_cast<float>( this->height() ) - wieverY );

	if ( isClick != mIsClicAruco ) {
		mIsClicAruco = isClick;

		if ( isClick ) {
			this->getEventQueue()->mouseButtonPress( wieverX, wieverY,button );
			this->getEventQueue()->mouseMotion( wieverX, wieverY );
		}
		else {
			this->getEventQueue()->mouseButtonRelease( wieverX, wieverY, button );
			return;
		}
	}
	this->getEventQueue()->mouseMotion( wieverX, wieverY );
}
