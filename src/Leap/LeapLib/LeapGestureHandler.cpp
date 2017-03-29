#include <easylogging++.h>

#include "LeapLib/LeapGestureHandler.h"
#include "LeapLib/LeapActions.h"

Leap::LeapGestureHandler::LeapGestureHandler( LeapManager* leapManager )
{
    leapActions = new Leap::LeapActions( leapManager );
	this->leapManager = leapManager;
    LOG( INFO ) << "LeapGestureHandler() Constructor";
}

Leap::LeapGestureHandler::~LeapGestureHandler()
{
	if ( leapManager != NULL ) {
		delete( leapManager );
	}
    LOG( INFO ) << "~LeapGestureHandler() Destructor";
}

void Leap::LeapGestureHandler::handleGestures( Frame frame )
{
    const GestureList gestures = frame.gestures();
    for (int g = 0; g < gestures.count(); ++g) {
        Gesture gesture = gestures[g];

        HandList hands = gesture.hands();
        Hand firstHand = hands[0];

        switch (gesture.type()) {
        case Gesture::TYPE_CIRCLE:
        {
            LOG( INFO ) << "GESTO CIRCLE....zoomGraph().";
            leapActions->zoomGraph(gesture);
            break;
        }
        case Gesture::TYPE_SWIPE:
        {

            LOG( INFO ) << "GESTO swipe ....rotateGraph().";
            leapActions->rotateGraph(gesture);
            break;
        }
        case Gesture::TYPE_KEY_TAP:
        {
            LOG( INFO ) << "GESTO KEY_TAP....onKeyTap().";
            leapActions->onKeyTap(gesture);
            break;
        }
        case Gesture::TYPE_SCREEN_TAP:
        {
            LOG( INFO ) << "GESTO SCREEN_TAP....onScreenTap().";
            leapActions->onScreenTap(gesture);
            break;
        }
        default:
            LOG( INFO ) << "Unknown gesture type.";
            break;
        }
    }
}
