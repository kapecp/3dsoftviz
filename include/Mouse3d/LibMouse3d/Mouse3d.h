#ifndef MOUSE3D_H
#define MOUSE3D_H

#include <QtGlobal>

#include <QApplication>
#include <QDebug>

#include "QOSG/CoreWindow.h"
#include "Application/Application.h"

#if defined(Q_OS_WIN)
	#include "Mouse3d/LibMouse3d/LibCoreMouse3d/Mouse3DInput.h"
#elif defined(Q_OS_LINUX)

#elif defined(Q_OS_MAC)
#endif

namespace QOSG{
    class CoreWindow;
}

namespace LibMouse3d{

class Mouse3dDevice : public QObject {
	Q_OBJECT

public:
	Mouse3dDevice( QOSG::CoreWindow *window );
    ~Mouse3dDevice();

#if defined(Q_OS_WIN)
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_MAC)
#endif

public slots:
//#if defined(Q_OS_WIN)
//#elif defined(Q_OS_LINUX)

	void translateX11Event( XEvent *event );

	void translateDummy();

//#elif defined(Q_OS_MAC)
//#endif


private:
	QOSG::CoreWindow* window;

#if defined(Q_OS_WIN)

	Mouse3DInput* mouse;

#elif defined(Q_OS_LINUX)

	Display *display;
	int initMouse3DLinux();

#elif defined(Q_OS_MAC)
#endif

};
}

#endif

