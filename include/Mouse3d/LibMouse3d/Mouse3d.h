#ifndef MOUSE3D_H
#define MOUSE3D_H

#include "Mouse3d/LibMouse3d/EventThread.h"
#include <QtGlobal>
#include <QDebug>

#if defined(Q_WS_X11)
    #include <stdlib.h>
    #include <stdio.h>

    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/Xos.h>
    #include <X11/Xatom.h>
    #include <X11/keysym.h>

    #include "xdrvlib.h"
#endif

namespace QOSG{
    class CoreWindow;
}

namespace LibMouse3d{

class Mouse3dDevice{

public:
    Mouse3dDevice(QOSG::CoreWindow *window);
    ~Mouse3dDevice();

#ifdef Q_WS_WIN
    Mouse3DInput* mouse;
#elif defined(Q_WS_X11)

//virtual void Move3d(std::vector<float>& motionData);
signals:

    void Move3d(std::vector<float>& motionData);

#elif defined(Q_WS_MAC)

#endif
    QOSG::CoreWindow* win;

private:
#if defined (Q_WS_X11)
    void Mouse3DLinux();
    std::vector<float> signal_data;
#endif

};
}

#endif

