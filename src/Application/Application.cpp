#include "Application/Application.h"

#include <QDebug>

#include <X11/Xlib.h>
#include "fixx11.h"

namespace App {

ApplicationX11::ApplicationX11( int &argc, char **argv ) :
	QApplication( argc, argv ),
	isConnected( false ) {
	//XInitThreads(); //crashes app
	//XSelectInput(QX11Info::display(), DefaultRootWindow(QX11Info::display()), SubstructureNotifyMask);
}

ApplicationX11::~ApplicationX11() {
}

bool ApplicationX11::x11EventFilter( XEvent *event ) {
	if ( this->isConnected && event->type == ClientMessage ){
		//qDebug() << "x11EventFilter : eventType = " << event->type;
		//XEvent xev = *event;
		//emit passX11Event( event );
		//emit passDummy();

		//Cheap, dirty but working solution
		device->translateX11Event( event );
	}
    return false;
}

void ApplicationX11::initConnection( LibMouse3d::Mouse3dDevice *device ){
	this->device = device;
	//QObject::connect( this, SIGNAL( passDummy( )), device, SLOT( translateDummy( )));
	//QObject::connect( this, SIGNAL( passX11Event( XEvent * )), device, SLOT( translateX11Event( XEvent * )));
	this->isConnected = true;
}

void ApplicationX11::closeConnection() {
	this->isConnected = false;
}

} //App
