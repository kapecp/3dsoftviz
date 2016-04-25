#include "Leap/LeapController.h"

void Leap::LeapController::startListening()
{
    enableGesture(Leap::Gesture::TYPE_SWIPE);
    addListener( leapListener );
}

void Leap::LeapController::stopListening()
{
	removeListener( leapListener );
}
