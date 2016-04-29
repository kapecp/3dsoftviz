#include "Mouse3d/LibMouse3d/Unix/ApplicationEmitter.h"

#include <QDebug>

#include <X11/Xlib.h>
#include "fixx11.h"

ApplicationEmitter::ApplicationEmitter( Mouse3DUnixDevice *device ) :
	QObject(),
	device( device ) {
	//QObject::connect( this, SIGNAL( signalDummy( )), device, SLOT( translateDummy( )));
	//QObject::connect( this, SIGNAL( signalX11Event( XEvent * )), device, SLOT( translateX11Event( XEvent * )));
}

ApplicationEmitter::~ApplicationEmitter(){
}

emitX11Event( XEvent *event ){
	//emit signalDummy();
	emit signalX11Event( event );
}