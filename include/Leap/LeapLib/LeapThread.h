#ifndef LEAPTHREAD_H
#define LEAPTHREAD_H

#include "Leap.h"
#include "LeapLib/LeapController.h"
#include "LeapLib/LeapManager.h"
#include "LeapLib/LeapExport.h"

#include <QObject>
#include <QThread>

namespace LeapLib {

/**
 * @author Peter Sutarik
 * @brief The LeapThread class
 * @brief Thread for processing LeapMotion data
 */
class LEAPLIB_EXPORT LeapThread : public QThread
{
	//Q_OBJECT
	//TODO resolve
public:
	LeapThread( QObject* parent=NULL, LeapManager* leapManager=NULL );
	~LeapThread( void );
	void run();

	LeapController* leapController;

};
}
#endif // LEAPTHREAD_H
