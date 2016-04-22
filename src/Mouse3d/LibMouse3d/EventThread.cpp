#include "Mouse3d/LibMouse3d/EventThread.h"

#include <QDebug>

#include "QOSG/CoreWindow.h"

#include "Mouse3d/LibMouse3d/Mouse3d.h"

namespace LibMouse3d{

EventThread::EventThread(QOSG::CoreWindow* window, QObject* parent) :
	QThread( parent ),
	window( window ) {
	qDebug() << "EventThread::EventThread(): winId() = " << this->window->winId() << endl;
	qDebug() << "EventThread::EventThread(): CurrentThreadId() = " << QThread::currentThreadId() << endl;
}

void EventThread::run(){
	qDebug() << "EventThread::run(): winId() = " << this->window->winId() << endl;
	qDebug() << "EventThread::run(): CurrentThreadId() = " << QThread::currentThreadId() << endl;
	mouseDevice = new Mouse3dDevice( this->window );
}

EventThread::~EventThread() {
    qDebug() << "Mouse3d thread destroyed";
}

void EventThread::msleep(unsigned long msecs){
    QThread::msleep(msecs);
}

/**
 * @author Michal Fasanek
 * @brief LibMouse3d::LibCoreMouse3d::EventThread::TerminateThread
 * @brief Called from application - terminates thread and connection to the device
 */


}
