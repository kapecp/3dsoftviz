#include "Mouse3d/LibMouse3d/ConnectionInterface.h"
#include "Mouse3d/LibMouse3d/LibCoreMouse3d/EventThread.h"
#include <QDebug>


LibMouse3d::ConnectionInterface::ConnectionInterface() {

}

LibMouse3d::ConnectionInterface::~ConnectionInterface() {

}

void LibMouse3d::ConnectionInterface::CreateConnection(){

    qDebug() << "Create connection call - ConnectionInterface";

    LibMouse3d::LibCoreMouse3d::EventThread* thread;// = new LibMouse3d::LibCoreMouse3d::EventThread::EventThread();
    thread = new LibMouse3d::LibCoreMouse3d::EventThread();
    thread->start();

}
