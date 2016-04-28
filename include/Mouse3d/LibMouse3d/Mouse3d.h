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
	#include <QX11Info>
	#include <vector>

	#include <X11/X.h>
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/Xatom.h>
	#include <X11/keysym.h>

	extern "C" {
		#include "xdrvlib.h"
	}

	#undef Unsorted
	#undef None
	//#undef Bool
	#undef KeyPress
	#undef KeyRelease
	#undef Above
	#undef Below
	#undef FocusIn
	#undef FocusOut
	#undef Always
	#undef Success
	#undef GrayScale
	#undef Status
	#undef CursorShape
	#undef FontChange
	#undef NormalState
	#undef index
	#undef rindex
#elif defined(Q_OS_MAC)
#endif

namespace QOSG{
    class CoreWindow;
}

namespace LibMouse3d{

class Mouse3dDevice : public QObject {
    //Q_OBJECT

public:
	Mouse3dDevice( QOSG::CoreWindow *window );
    ~Mouse3dDevice();

#if defined(Q_OS_WIN)
#elif defined(Q_OS_LINUX)
#elif defined(Q_OS_MAC)
#endif

public slots:
#if defined(Q_OS_WIN)
#elif defined(Q_OS_LINUX)

	void x11TranslateEvent( XEvent &event );

#elif defined(Q_OS_MAC)
#endif


private:
	QOSG::CoreWindow* window;

#if defined(Q_OS_WIN)

	Mouse3DInput* mouse;

#elif defined(Q_OS_LINUX)

    void Mouse3DLinux();
	void x11Mouse3DInit();

#elif defined(Q_OS_MAC)
#endif

};
}

#endif

