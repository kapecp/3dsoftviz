#include "LeapLib/LeapController.h"

Leap::LeapController::LeapController( LeapManager* leapManager )
{
	leapListener = new Leap::LeapListener( leapManager );
}

Leap::LeapController::~LeapController()
{
	if ( leapListener != NULL ) {
		delete( leapListener );
	}
}

void Leap::LeapController::startListening()
{
	enableGesture( Leap::Gesture::TYPE_SWIPE );
	setPolicy( Leap::Controller::POLICY_IMAGES );
	addListener( *leapListener );
}

void Leap::LeapController::stopListening()
{
	removeListener( *leapListener );
}
