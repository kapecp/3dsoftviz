#include "Application/Application.h"

#include <QDebug>

#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
	#include <X11/Xlib.h>
	#include "fixx11.h"
#endif

namespace App {

Application::Application( int &argc, char **argv ) : QApplication( argc, argv ) {
	#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
		//XInitThreads(); //crashes app
		//XSelectInput(QX11Info::display(), DefaultRootWindow(QX11Info::display()), SubstructureNotifyMask); // possibly could improve performance, filtering xevents
	#endif
}

Application::~Application() {
}

#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
bool Application::x11EventFilter( XEvent *event ) {
	if ( this->isEmitting && event->type == ClientMessage ){
		//qDebug() << "x11EventFilter : eventType = " << event->type;
		//XEvent xev = *event;
		//emit passX11Event( event );
		//emit passDummy();

		//Cheap, dirty but working solution
		//device->translateX11Event( event );
		this->emitter->emitX11Event( event );
	}
    return false;
}

void Application::startEmitter( Mouse3DUnixDevice *device ){
	this->emitter = new ApplicationEmitter( device );
	//QObject::connect( emitter, SIGNAL( signalDummy( )), device, SLOT( translateDummy( )));
	//QObject::connect( emitter, SIGNAL( signalX11Event( XEvent * )), device, SLOT( translateX11Event( XEvent * )));
	this->isEmitting = true;
}

void Application::stopEmitter() {
	this->isEmitting = false;
	delete this->emitter;
}
#endif

} //App
