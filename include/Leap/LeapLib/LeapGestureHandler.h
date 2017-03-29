#ifndef LEAPGESTUREHANDLER_H
#define LEAPGESTUREHANDLER_H
#include "Leap.h"
#include "LeapLib/LeapManager.h"
#include "LeapLib/LeapActions.h"

#include "easylogging++.h"

namespace Leap {

class LeapGestureHandler{
public:

    LeapGestureHandler( LeapManager* leapManager );
    ~LeapGestureHandler();
    void handleGestures( Frame frame );

    LeapManager* leapManager;
    LeapActions* leapActions;
};
}


#endif // LEAPGESTUREHANDLER_H
