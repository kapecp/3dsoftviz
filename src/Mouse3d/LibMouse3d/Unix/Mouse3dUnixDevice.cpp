#include "Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.h"

#include <QDebug>
#include <QX11Info>

#include <easylogging++.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>

#define SGI // Required by xdrvlib.h
extern "C" {
#include "xdrvlib.h"
}

#include "fixx11.h"

Mouse3dUnixDevice::Mouse3dUnixDevice( /*QWidget *window*/ QOSG::CoreWindow* window ) : QObject(),
	window( window )
{

	this->display = QX11Info::display();
}

Mouse3dUnixDevice::~Mouse3dUnixDevice()
{
	QCoreApplication* inst = App::Application::instance();
	App::Application* app = qobject_cast<App::Application*>( inst );
	app->stopEmitter();
}

void Mouse3dUnixDevice::initMouse3d()
{
	Window xwindow = this->window->winId();

	if ( !MagellanInit( this->display, xwindow ) ) {
		LOG( INFO ) << "[X11] Mouse3dDevice: MagellanInit() = Failure!" << endl;
		LOG( INFO ) << "[X11] Mouse3dDevice: No driver is running. Exit ... " << endl;
		exit( EXIT_FAILURE );
	}
	else {
		LOG( INFO ) << "[X11] Mouse3dDevice: MagellanInit() = Success!" << endl;
	}

	QCoreApplication* inst = App::Application::instance();
	App::Application* app = qobject_cast<App::Application*>( inst );
	//QObject::connect( app, SIGNAL(passDummy()), this, SLOT(translateDummy()));
	app->startEmitter( this );
}

void Mouse3dUnixDevice::translateX11Event( XEvent* event )
{
	qDebug() << "[X11] Mouse3dDevice: eventType =" << event->type;

	MagellanFloatEvent MagellanEvent;
	if ( MagellanTranslateEvent( this->display, event, &MagellanEvent, 1.0, 1.0 ) == MagellanInputMotionEvent ) {
		MagellanRemoveMotionEvents( display );

		std::vector<double> motionData;
		motionData.push_back( MagellanEvent.MagellanData[ MagellanX ] );
		motionData.push_back( MagellanEvent.MagellanData[ MagellanY ] );
		motionData.push_back( MagellanEvent.MagellanData[ MagellanZ ] * -1.0 );
		motionData.push_back( MagellanEvent.MagellanData[ MagellanA ] );
		motionData.push_back( MagellanEvent.MagellanData[ MagellanC ] * -1.0 );
		motionData.push_back( MagellanEvent.MagellanData[ MagellanB ] * -1.0 );

		// Axi X,Y,Z scaling
		motionData[0] /= 100.0;
		motionData[1] /= 100.0;
		motionData[2] /= 100.0;

//		// Rotation A,B,C normalization to <0.0 - 1.0>
//		float normAxis = sqrt(motionData[3] * motionData[3] + motionData[4] * motionData[4] + motionData[5] * motionData[5]);
//		if (normAxis == 0.0){
//			motionData[3] = 0.0;
//			motionData[4] = 0.0;
//			motionData[5] = 1.0;
//		}
//		else{
//			motionData[3] /= normAxis;
//			motionData[4] /= normAxis;
//			motionData[5] /= normAxis;
//		}

//		motionData[3] /= 10.0;
//		motionData[4] /= 10.0;
//		motionData[5] /= 10.0;


		// Rotation A,B,C scaling
		motionData[3] /= 7000.0;
		motionData[4] /= 7000.0;
		motionData[5] /= 7000.0;

		qDebug() << "Movement: x =" << motionData[0] <<
				 "y =" << motionData[1] <<
				 "z =" << motionData[2];
		qDebug() << "Rotation: a =" << motionData[3] <<
				 "b =" << motionData[4] <<
				 "c =" << motionData[5] << endl;

		//emit Mouse3dDevices::Move3d(motionData);
		window->OnMove( motionData );
	}
}

void Mouse3dUnixDevice::translateDummy()
{
	qDebug() << "[X11] Mouse3dDevice: translateDummy called";
}
