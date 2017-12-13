#ifndef LEAPCONTROLLER_H
#define LEAPCONTROLLER_H


#include "LeapLib/LeapListener.h"
#include "LeapLib/LeapManager.h"
#include "LeapLib/LeapExport.h"

#include "Leap.h"

namespace LeapLib {

/**
 * @author Filip Mikle
 * @brief The Leap controller class
 * @brief Controls when the leap sensor starts and stops
 */
class LEAPLIB_EXPORT LeapController : public Leap::Controller
{
public:
	LeapListener* leapListener;

	LeapController( LeapManager* leapManager );
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
