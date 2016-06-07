#include "Mouse3d/Connector.h"

#include <QThread>

#include "QOSG/CoreWindow.h"

Mouse3d::Connector::Connector( QOSG::CoreWindow* window ):
	window( window )
{

}

Mouse3d::Connector::~Connector()
{
	delete thread;
}

/**
 * @brief LibMouse3d::ConnectionInterface::CreateConnection
 * @param wHndl window handle ID passed ad int32_t -> Windows platform-specific
 * Pass connection call to device library
 */
void Mouse3d::Connector::CreateConnection()
{
	//Create thread for signals from 3D mouse
	thread = new LibMouse3d::EventThread( window );
	thread->start();
}

