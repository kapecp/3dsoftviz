#ifndef LEAPGESTUREHANDLER_H
#define LEAPGESTUREHANDLER_H

#include "LeapLib/LeapManager.h"
#include "LeapLib/LeapActions.h"

#include "easylogging++.h"
#include "Leap.h"

namespace LeapLib {

class LeapGestureHandler
{
public:

	LeapGestureHandler( LeapManager* leapManager );
	~LeapGestureHandler();
	void handleGestures( Leap::Frame frame );

	LeapManager* leapManager;
	LeapActions* leapActions;
private:
	Leap::Frame* startFrame;
	bool isStartFrameSet;
	int frameCounter;

	void setStartFrame( Leap::Frame frame );
};
}


#endif // LEAPGESTUREHANDLER_H
