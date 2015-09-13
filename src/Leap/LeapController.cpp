#include "Leap/LeapController.h"

void Leap::LeapController::startListening()
{
	addListener( leapListener );
}

void Leap::LeapController::stopListening()
{
	removeListener( leapListener );
}
