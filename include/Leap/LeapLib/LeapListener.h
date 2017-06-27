#ifndef LEAPLISTENER_H
#define LEAPLISTENER_H


#include "LeapLib/LeapGestureHandler.h"

#include "LeapLib/LeapManager.h"
#include "CustomLeapManager.h"
#include "LeapLib/LeapExport.h"

#include "Leap.h"

namespace Leap {

class LeapListener;

/**
 * @author Filip Mikle
 * @brief The Leap listener class
 * @brief Overrides listener actions from default Listener with custom code
 */
class LEAPLIB_EXPORT LeapListener : public Listener
{
public:

	/**
	 * @brief Class that contains Actions that should be executed after a specific gesture
	 */

	LeapGestureHandler* leapGestureHandler;
	CustomLeapManager* leapManager;
	bool arMode;

	LeapListener( LeapManager* leapManager );
	~LeapListener( void );

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
	void onImages( const Controller& controller );
};
}

#endif
