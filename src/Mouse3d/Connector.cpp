#include "Mouse3d/Connector.h"
#include <QDebug>
#include "QOSG/CoreWindow.h"
#include "Mouse3d/LibMouse3d/EventThread.h"

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
    thread = new LibMouse3d::EventThread(this->win);
    thread->start();
}

