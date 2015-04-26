#ifndef LEAPLISTENER_H
#define LEAPLISTENER_H
#include "Leap.h"
#include "Leap/LeapActions.h"

namespace Leap {

class LeapListener;

/**
 * @author Filip Mikle
 * @brief The Leap listener class
 * @brief Overrides listener actions from default Listener with custom code
 */
class LeapListener : public Listener
{
public:

	/**
	 * @brief Class that contains Actions that should be executed after a specific gesture
	 */
	LeapActions leapActions;

	void onInit( const Controller& );
	void onConnect( const Controller& );
	void onDisconnect( const Controller& );
	void onExit( const Controller& );
	void onFrame( const Controller& );
	void onFocusGained( const Controller& );
	void onFocusLost( const Controller& );
	void onDeviceChange( const Controller& );
	void onServiceConnect( const Controller& );
	void onServiceDisconnect( const Controller& );
};
}

#endif
