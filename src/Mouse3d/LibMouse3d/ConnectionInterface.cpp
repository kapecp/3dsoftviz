#include "Mouse3d/LibMouse3d/ConnectionInterface.h"
#include "Mouse3d/LibMouse3d/LibCoreMouse3d/EventThread.h"
#include <QDebug>


LibMouse3d::ConnectionInterface::ConnectionInterface(QOSG::CoreWindow* window) {
    this->win = window;
}

LibMouse3d::ConnectionInterface::~ConnectionInterface() {

}
/**
 * @brief LibMouse3d::ConnectionInterface::CreateConnection
 * @param wHndl window handle ID passed ad int32_t -> Windows platform-specific
 * Pass connection call to device library
 */
void LibMouse3d::ConnectionInterface::CreateConnection(){

    qDebug() << "Create connection call - ConnectionInterface";


    thread = new LibMouse3d::LibCoreMouse3d::EventThread(this->win);
    thread->start();
}

