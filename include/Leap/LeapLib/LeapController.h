#ifndef LEAPCONTROLLER_H
#define LEAPCONTROLLER_H

#include "Leap.h"
#include "LeapLib/LeapListener.h"
#include "LeapLib/LeapCameraManipulator.h"
#include "LeapLib/LeapExport.h"

namespace Leap {

/**
 * @author Filip Mikle
 * @brief The Leap controller class
 * @brief Controls when the leap sensor starts and stops
 */
class LEAPLIB_EXPORT LeapController:Controller
{
public:
	LeapListener* leapListener;

	LeapController(LeapCameraManipulator* cameraManipulator);
	~LeapController( void );

	/**
	 * @brief After this action is executed the leap sensor starts to listen on events
	 */
	void startListening();

	/**
	 * @brief After this action is executed the leap sensor stops listening
	 */
	void stopListening();
};
}

#endif // LEAPCONTROLLER_H
