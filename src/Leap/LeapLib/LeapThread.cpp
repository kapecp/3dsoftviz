#include <QDebug>
#include "LeapLib/LeapThread.h"

Leap::LeapThread::LeapThread( QObject* parent, LeapCameraManipulator* cameraManipulator) : QThread( parent )
{
	qDebug() << "LeapThread::LeapThread, created";
	leapController = new LeapController(cameraManipulator);
}

Leap::LeapThread::~LeapThread( void )
{
	leapController->stopListening();
	delete(leapController);
	qDebug() << "LeapThread::~LeapThread, destroyed";
}

void Leap::LeapThread::run()
{
	qDebug() << "[Leap::LeapThread::run()]";
	leapController->startListening();
}
