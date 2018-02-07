#ifndef LEAPLISTENER_H
#define LEAPLISTENER_H


#include "LeapLib/LeapGestureHandler.h"

#include "LeapLib/LeapManager.h"
#include "CustomLeapManager.h"
#include "LeapLib/LeapExport.h"

#include "Leap.h"

namespace LeapLib {

class LeapListener;

/**
 * @author Filip Mikle
 * @brief The Leap listener class
 * @brief Overrides listener actions from default Listener with custom code
 */
class LEAPLIB_EXPORT LeapListener : public Leap::Listener
{
public:

	/**
	 * @brief Class that contains Actions that should be executed after a specific gesture
	 */

	LeapGestureHandler* leapGestureHandler;
	Softviz::Leap::CustomLeapManager* leapManager;
	bool arMode;

	LeapListener( LeapManager* leapManager );
	~LeapListener( void );

	void onInit( const Leap::Controller& );
	void onConnect( const Leap::Controller& );
	void onDisconnect( const Leap::Controller& );
	void onExit( const Leap::Controller& );
	void onFrame( const Leap::Controller& );
	void onFocusGained( const Leap::Controller& );
	void onFocusLost( const Leap::Controller& );
	void onDeviceChange( const Leap::Controller& );
	void onServiceConnect( const Leap::Controller& );
	void onServiceDisconnect( const Leap::Controller& );
	void onImages( const Leap::Controller& controller );
};
}

#endif
