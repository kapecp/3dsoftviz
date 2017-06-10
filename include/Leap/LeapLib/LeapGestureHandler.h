#ifndef LEAPGESTUREHANDLER_H
#define LEAPGESTUREHANDLER_H

#include "LeapLib/LeapManager.h"
#include "LeapLib/LeapActions.h"

#include "easylogging++.h"
#include "Leap.h"
namespace Leap {

class LeapGestureHandler
{
public:

	LeapGestureHandler( LeapManager* leapManager );
	~LeapGestureHandler();
	void handleGestures( Frame frame );

	LeapManager* leapManager;
	LeapActions* leapActions;
private:
	Frame* startFrame;
	bool isStartFrameSet;
	int frameCounter;

	void setStartFrame( Frame frame );
};
}


#endif // LEAPGESTUREHANDLER_H
