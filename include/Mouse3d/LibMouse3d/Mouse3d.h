#ifndef MOUSE3D_H
#define MOUSE3D_H

#include <QApplication>

#if defined(Q_OS_WIN)

	#include "Mouse3d/LibMouse3d/LibCoreMouse3d/Mouse3DInput.h"

#elif defined(Q_OS_LINUX)

	#include <QX11Info>

	#include <vector>

    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/Xos.h>
    #include <X11/Xatom.h>
    #include <X11/keysym.h>

    #include "xdrvlib.h"

#elif defined(Q_OS_MAC)

	// Mac #include here

#endif

namespace QOSG{
    class CoreWindow;
}

namespace LibMouse3d{

class Mouse3dDevice : public QApplication {

    //Q_OBJECT

public:
    Mouse3dDevice(int argc, char ** argv, QOSG::CoreWindow *window);
    ~Mouse3dDevice();

#if defined(Q_OS_WIN)

#elif defined(Q_OS_LINUX)

	void SendSignal(std::vector<float>& motionData);

    virtual bool x11EventFilter(XEvent *event);

#elif defined(Q_OS_MAC)

	// Mac public here

#endif

signals:
#if defined(Q_OS_WIN)

#elif defined(Q_OS_LINUX)

	void Move3d(std::vector<float>& motionData);

#elif defined(Q_OS_MAC)

	// Mac signals here

#endif


private:
	QOSG::CoreWindow* window;

#if defined(Q_OS_WIN)

	Mouse3DInput* mouse;

#elif defined(Q_OS_LINUX)

	void Mouse3DLinux(QOSG::CoreWindow* window);

#elif defined(Q_OS_MAC)

	// Mac private here

#endif

};
}

#endif

