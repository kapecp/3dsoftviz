#include "Mouse3d/LibMouse3d/EventThread.h"

#include <easylogging++.h>

#include "QOSG/CoreWindow.h"

#include "Mouse3d/LibMouse3d/Mouse3dDevices.h"

namespace LibMouse3d {

EventThread::EventThread( QOSG::CoreWindow* window, QObject* parent ) :
	QThread( parent ),
	window( window )
{
}

void EventThread::run()
{
	mouseDevice = new Mouse3dDevices( this->window );
	LOG( INFO ) << "Mouse3dDevices thread created";
}

EventThread::~EventThread()
{
	delete mouseDevice;
	LOG( INFO ) << "Mouse3dDevices thread destroyed";
}

void EventThread::msleep( unsigned long msecs )
{
	QThread::msleep( msecs );
}

/**
 * @author Michal Fasanek
 * @brief LibMouse3d::LibCoreMouse3d::EventThread::TerminateThread
 * @brief Called from application - terminates thread and connection to the device
 */


}
