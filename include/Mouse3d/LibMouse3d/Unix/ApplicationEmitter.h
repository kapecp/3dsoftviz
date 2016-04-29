#pragma once

#include <QObject>

#include "Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.h"

class ApplicationEmitter : public QObject {
	Q_OBJECT

public:
	ApplicationEmitter( Mouse3DUnixDevice *device );

	~ApplicationEmitter();
	
	emitX11Event( XEvent *event );
	
signals:
	 void signalX11Event( XEvent *event );

	 void signalDummy();

private:
	 Mouse3DUnixDevice *device;
};
