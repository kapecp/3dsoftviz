#ifndef LEAPTHREAD_H
#define LEAPTHREAD_H

#include "Leap.h"
#include "LeapLib/LeapController.h"
#include "LeapLib/LeapCameraManipulator.h"
#include "LeapLib/LeapExport.h"

#include <QObject>
#include <QThread>

namespace Leap {

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
	LeapThread( QObject* parent=NULL, LeapCameraManipulator* cameraManipulator=NULL);
	~LeapThread( void );
	void run();

	LeapController* leapController;

};
}
#endif // LEAPTHREAD_H
