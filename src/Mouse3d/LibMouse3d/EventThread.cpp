#include "Mouse3d/LibMouse3d/EventThread.h"
#include "QOSG/CoreWindow.h"
#include "Mouse3d/LibMouse3d/Mouse3d.h"

namespace LibMouse3d{

EventThread::EventThread(QOSG::CoreWindow* window, QObject* parent) : QThread( parent ) {

    this->mouseDevice = new Mouse3dDevice(window);
    //vetvenie pre platform independent build


}

EventThread::~EventThread() {
    qDebug() << "Mouse3d thread destroyed";
}

/**
 * @author Michal Fasanek
 * @brief LibMouse3d::LibCoreMouse3d::EventThread::TerminateThread
 * @brief Called from application - terminates thread and connection to the device
 */

}
