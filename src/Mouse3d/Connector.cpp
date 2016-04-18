#include "Mouse3d/Connector.h"

Mouse3d::Connector::Connector(QOSG::CoreWindow* window){
    this->win = window;
}



Mouse3d::Connector::~Connector(){

}

/**
 * @brief LibMouse3d::ConnectionInterface::CreateConnection
 * @param wHndl window handle ID passed ad int32_t -> Windows platform-specific
 * Pass connection call to device library
 */
void Mouse3d::Connector::CreateConnection(){

    //Create thread for signals from 3D mouse
	qDebug() << "Connector::CreateConnection(): winId() = " << this->win->winId() << endl;
	qDebug() << "Connector::CreateConnection(): CurrentThreadId() = " << QThread::currentThreadId() << endl;
    thread = new LibMouse3d::EventThread(this->win);
    thread->start();
}

