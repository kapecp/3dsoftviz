#include "Mouse3d/LibMouse3d/Unix/ApplicationEmitter.h"

//#include <QDebug>
//#include <easylogging++.h>

#include <X11/Xlib.h>
#include "fixx11.h"

ApplicationEmitter::ApplicationEmitter( Mouse3dUnixDevice* device ) :
	QObject(),
	device( device )
{
	//QObject::connect( this, SIGNAL( signalDummy( )), device, SLOT( translateDummy( )));
	//QObject::connect( this, SIGNAL( signalX11Event( XEvent * )), device, SLOT( translateX11Event( XEvent * )));
}

ApplicationEmitter::~ApplicationEmitter()
{
}

#if QT_VERSION >= 0x050000
//todo
#elif QT_VERSION < 0x050000
void ApplicationEmitter::emitX11Event( XEvent* event )
{
	//qDebug() << "emitX11Event : eventType = " << event->type;
	//XEvent xev = *event;
	//emit signalDummy();
	//emit signalX11Event( event );

	//Cheap, dirty but working solution
	device->translateX11Event( event );
}
#endif
