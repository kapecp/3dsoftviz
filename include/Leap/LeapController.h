#ifndef LEAPCONTROLLER_H
#define LEAPCONTROLLER_H

#include "Leap.h"
#include "Leap/LeapListener.h"

namespace Leap {

/**
 * @author Filip Mikle
 * @brief The Leap controller class
 * @brief Controls when the leap sensor starts and stops
 */
class LeapController:Controller
{
public:
	LeapListener leapListener;

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
