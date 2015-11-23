#ifndef LEAPTHREAD_H
#define LEAPTHREAD_H

#include "Leap.h"
#include "Leap/LeapController.h"

#include <QObject>
#include <QThread>

namespace Leap {

/**
 * @author Peter Sutarik
 * @brief The LeapThread class
 * @brief Thread for processing LeapMotion data
 */
class LeapThread : public QThread
{
	Q_OBJECT

public:
	LeapThread( QObject* parent=NULL );
	~LeapThread( void );
	void run();

	LeapController leapController;

};
}
#endif // LEAPTHREAD_H
