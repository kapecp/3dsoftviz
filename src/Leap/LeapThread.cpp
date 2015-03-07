#include "Leap/LeapThread.h"

Leap::LeapThread::LeapThread( QObject* parent ) : QThread( parent )
{
    qDebug() << "LeapThread::LeapThread, created";
}

Leap::LeapThread::~LeapThread( void )
{
    qDebug() << "LeapThread::~LeapThread, destroyed";
}

void Leap::LeapThread::run()
{
    qDebug() << "[Leap::LeapThread::run()]";
    LeapController leapController;
    leapController.startListening();
}
