#ifndef MOUSE3D_H
#define MOUSE3D_H

#include <QtGlobal>

#include <QApplication>

#include "QOSG/CoreWindow.h"
#include "Application/Application.h"

#if defined(Q_OS_WIN)
	#include "Mouse3d/LibMouse3d/Windows/Mouse3DInput.h"
#elif defined(Q_WS_X11) || defined(Q_OS_LINUX)
	#include "Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.h"
#elif defined(Q_OS_MAC)
#endif

namespace QOSG{
    class CoreWindow;
}

namespace LibMouse3d{

class Mouse3dDevices : public QObject {
	Q_OBJECT

public:
	Mouse3dDevices( QOSG::CoreWindow *window );
	~Mouse3dDevices();

private:
	QOSG::CoreWindow* window;

#if defined(Q_OS_WIN)

	Mouse3DInput* mouse;

#elif defined(Q_WS_X11) || defined(Q_OS_LINUX)

	Mouse3DUnixDevice* mouse;

#elif defined(Q_OS_MAC)
#endif

};
}

#endif

