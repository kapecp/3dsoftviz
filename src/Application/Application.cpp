#include "Application/Application.h"
#include <QtGlobal>
#include <QDebug>
#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
#if QT_VERSION >= 0x050000
//todo
#elif QT_VERSION < 0x050000
#include <X11/Xlib.h>
#include "fixx11.h"
#endif
#endif

namespace App {

Application::Application( int& argc, char** argv ) : QApplication( argc, argv )
{
#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
	this->isEmitting = false;
	//XInitThreads(); //crashes app
	//XSelectInput(QX11Info::display(), DefaultRootWindow(QX11Info::display()), SubstructureNotifyMask); // possibly could improve performance, filtering xevents
#endif
}

Application::~Application()
{
}

#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
#if QT_VERSION >= 0x050000
//todo
#elif QT_VERSION < 0x050000
bool Application::x11EventFilter( XEvent* event )
{
	//qDebug() << "x11EventFilter : eventType = " << event->type;
	if ( this->isEmitting && event->type == ClientMessage ) {
		this->emitter->emitX11Event( event );
	}
	return false;
}
#endif

void Application::startEmitter( Mouse3dUnixDevice* device )
{
	this->emitter = new ApplicationEmitter( device );
	//QObject::connect( emitter, SIGNAL( signalDummy( )), device, SLOT( translateDummy( )));
	//QObject::connect( emitter, SIGNAL( signalX11Event( XEvent * )), device, SLOT( translateX11Event( XEvent * )));
	this->isEmitting = true;
}

void Application::stopEmitter()
{
	this->isEmitting = false;
	delete this->emitter;
}
#endif
} //App
