#include "Mouse3d/LibMouse3d/EventThread.h"

namespace LibMouse3d{

EventThread::EventThread(QOSG::CoreWindow* window, QObject* parent) :
	QThread( parent ),
	window(window)
{
	qDebug() << "EventThread::EventThread(): winId() = " << this->window->winId() << endl;
}

void EventThread::run(){
	qDebug() << "EventThread::run(): winId() = " << this->window->winId() << endl;
	qDebug() << "EventThread::run(): CurrentThreadId() = " << QThread::currentThreadId() << endl;
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
