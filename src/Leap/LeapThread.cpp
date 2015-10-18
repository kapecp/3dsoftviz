#include <QDebug>
#include "Leap/LeapThread.h"

#include <QDebug>

Leap::LeapThread::LeapThread( QObject* parent ) : QThread( parent )
{
	qDebug() << "LeapThread::LeapThread, created";
}

Leap::LeapThread::~LeapThread( void )
{
	leapController.stopListening();
	qDebug() << "LeapThread::~LeapThread, destroyed";
}

void Leap::LeapThread::run()
{
	qDebug() << "[Leap::LeapThread::run()]";
	leapController.startListening();
}
