#include "Mouse3d/LibMouse3d/Mouse3d.h"
#include "Mouse3d/LibMouse3d/EventThread.h"
#include "QOSG/CoreWindow.h"


namespace LibMouse3d{

    Mouse3dDevice::Mouse3dDevice(QOSG::CoreWindow* window){
#ifdef Q_WS_WIN
        this->mouse = new Mouse3DInput(window);
        QObject::connect(mouse, SIGNAL(Move3d(std::vector<float>&)), window, SLOT(OnMove(std::vector<float>&)));
#elif Q_WS_X11

#elif Q_WS_MAC

#endif

    }

    Mouse3dDevice::~Mouse3dDevice(){

    }


}
