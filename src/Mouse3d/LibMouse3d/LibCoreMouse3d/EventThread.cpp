#include "Mouse3d/LibMouse3d/LibCoreMouse3d/EventThread.h"


using namespace LibMouse3d::LibCoreMouse3d;

EventThread::EventThread(QObject* parent ) : QThread( parent ) {

}

EventThread::~EventThread() {
    qDebug() << "Mouse3d thread destroyed";
}

/**
 * @author Michal Fasanek
 * @brief LibMouse3d::LibCoreMouse3d::EventThread::TerminateThread
 * @brief Called from application - terminates thread and connection to the device
 */


void EventThread::run(){


    qDebug() << "Mouse3d thread finished";
}

