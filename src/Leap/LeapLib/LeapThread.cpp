#include "LeapLib/LeapThread.h"
#include "easylogging++.h"

Leap::LeapThread::LeapThread( QObject* parent, LeapCameraManipulator* cameraManipulator) : QThread( parent )
{
	LOG(INFO) << "LeapThread::LeapThread, created";
	leapController = new LeapController(cameraManipulator);
}

Leap::LeapThread::~LeapThread( void )
{
	leapController->stopListening();
	delete(leapController);
	LOG(INFO) << "LeapThread::~LeapThread, destroyed";
}

void Leap::LeapThread::run()
{
	LOG(INFO) << "[Leap::LeapThread::run()]";
	leapController->startListening();
}
