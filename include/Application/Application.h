#pragma once

#include <QApplication>

#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
#include "Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.h"
#include "Mouse3d/LibMouse3d/Unix/ApplicationEmitter.h"
#endif

namespace App {

class Application : public QApplication
{
	Q_OBJECT

public:
	Application( int& argc, char** argv );

	~Application();

#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
	virtual bool x11EventFilter( XEvent* event );

	void startEmitter( Mouse3dUnixDevice* device );

	void stopEmitter();
#endif

#if defined(Q_WS_X11) || defined(Q_OS_LINUX)
private:
	bool isEmitting;
	ApplicationEmitter* emitter;
#endif

};
} //App
