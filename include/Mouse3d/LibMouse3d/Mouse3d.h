#ifndef MOUSE3D_H
#define MOUSE3D_H

#include <QtGlobal>
#include <QDebug>

#ifdef Q_OS_WIN
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
#endif

#include "QOSG/CoreWindow.h"

#include "Mouse3d/LibMouse3d/EventThread.h"

namespace QOSG{
    class CoreWindow;
}

namespace LibMouse3d{

class Mouse3dDevice : public QObject {


public:
    Mouse3dDevice(QOSG::CoreWindow *window);
    ~Mouse3dDevice();

#ifdef Q_OS_WIN
    Mouse3DInput* mouse;
#elif defined(Q_OS_LINUX)

void SendSignal(std::vector<float>& motionData);

signals:

    void Move3d(std::vector<float>& motionData);

#elif defined(Q_OS_MAC)

#endif
    QOSG::CoreWindow* win;

#if defined (Q_OS_LINUX)
    void Mouse3DLinux(QOSG::CoreWindow* window);
    std::vector<float> signal_data;
#endif

};
}

#endif

