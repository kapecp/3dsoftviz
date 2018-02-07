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
#if QT_VERSION >= 0x050000
//todo
#elif QT_VERSION < 0x050000
	void emitX11Event( XEvent* event );
#endif
signals:
#if QT_VERSION >= 0x050000
//todo
#elif QT_VERSION < 0x050000
	void signalX11Event( XEvent* event );
#endif
	void signalDummy();

private:
	Mouse3dUnixDevice* device;
};
