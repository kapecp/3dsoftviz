#include "Mouse3d/LibMouse3d/EventThread.h"
#include "QOSG/CoreWindow.h"
#include "Mouse3d/LibMouse3d/Mouse3d.h"

namespace LibMouse3d{

EventThread::EventThread(QOSG::CoreWindow* window, QObject* parent) : QThread( parent ) {
    this->window = window;
    qDebug() << window->winId();
}

void EventThread::run(){
    qDebug() << window->winId();
    this->mouseDevice = new Mouse3dDevice(window);
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
