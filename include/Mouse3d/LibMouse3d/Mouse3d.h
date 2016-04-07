#ifndef MOUSE3D_H
#define MOUSE3D_H

#include "Mouse3d/LibMouse3d/EventThread.h"
#include <QtGlobal>

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
#elif defined (Q_WS_X11)

#elif defined (Q_WS_MAC)

#endif
    QOSG::CoreWindow* win;
};

}

#endif
