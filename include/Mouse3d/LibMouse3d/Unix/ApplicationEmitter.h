#pragma once

#include <QObject>

#include "Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.h"
class Mouse3dUnixDevice;
class ApplicationEmitter;
class ApplicationEmitter : public QObject
{
	Q_OBJECT

public:
	ApplicationEmitter( Mouse3dUnixDevice* device );

	~ApplicationEmitter();

	void emitX11Event( XEvent* event );

signals:
	void signalX11Event( XEvent* event );

	void signalDummy();

private:
	Mouse3dUnixDevice* device;
};
