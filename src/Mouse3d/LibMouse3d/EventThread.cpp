#include "Mouse3d/LibMouse3d/EventThread.h"
#include "QOSG/CoreWindow.h"

namespace LibMouse3d{

EventThread::EventThread(QOSG::CoreWindow* window, QObject* parent) : QThread( parent ) {
    this->win = window;
    this->mouse = new Mouse3DInput(window);

    QObject::connect(mouse, SIGNAL(Move3d(std::vector<float>&)), window, SLOT(OnMove(std::vector<float>&)));


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
