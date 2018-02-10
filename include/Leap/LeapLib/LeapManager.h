#ifndef LEAPMANAGER_H
#define LEAPMANAGER_H
#include "LeapLib/LeapExport.h"

#include <Leap.h>

namespace LeapLib {

class LEAPLIB_EXPORT LeapManager
{
public:
	virtual ~LeapManager( void ) {}

	enum Movement {
		RIGHT, LEFT, UP, DOWN, FORWARD, BACKWARD
	};
	bool arMode;

	virtual void enableCameraMovement( Movement direction ) = 0;
	virtual void disableCameraMovement() = 0;
	virtual void rotateCamera( float py0, float px0, double throwScale, float py1, float px1 ) = 0;
	virtual void updateHands( Leap::Hand leftHand, Leap::Hand rightHand ) = 0;
	//jurik
	virtual void graphRotateSwipe( int swipeDirection ) = 0;
	virtual void scaleNodes( bool scaleUp ) = 0;
	virtual void rotateArucoLeft() = 0;
	virtual void rotateArucoRight() = 0;
	virtual void scaleEdgesUp() = 0;
	virtual void scaleEdgesDown() = 0;
	virtual void scaleGraph( bool scaleUp ) = 0 ;

	virtual void selectNode( bool right ) = 0;
	//*****
};
}
#endif
