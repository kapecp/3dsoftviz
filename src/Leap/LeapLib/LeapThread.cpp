#include <easylogging++.h>

#include "LeapLib/LeapThread.h"

LeapLib::LeapThread::LeapThread( QObject* parent, LeapManager* leapManager ) : QThread( parent )
{
	LOG( INFO ) << "LeapThread::LeapThread, created";
	leapController = new LeapController( leapManager );
}

LeapLib::LeapThread::~LeapThread( void )
{
	leapController->stopListening();
	delete ( leapController );
	LOG( INFO ) << "LeapThread::~LeapThread, destroyed";
}

void LeapLib::LeapThread::run()
{
	LOG( INFO ) << "[Leap::LeapThread::run()]";
	leapController->startListening();
}
