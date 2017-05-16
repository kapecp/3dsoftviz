#include <easylogging++.h>

#include "LeapLib/LeapThread.h"

Leap::LeapThread::LeapThread( QObject* parent, LeapManager* leapManager ) : QThread( parent )
{
	LOG( INFO ) << "LeapThread::LeapThread, created";
	leapController = new LeapController( leapManager );
}

Leap::LeapThread::~LeapThread( void )
{
	leapController->stopListening();
	delete ( leapController );
	LOG( INFO ) << "LeapThread::~LeapThread, destroyed";
}

void Leap::LeapThread::run()
{
	LOG( INFO ) << "[Leap::LeapThread::run()]";
	leapController->startListening();
}
