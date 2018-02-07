#include "LeapLib/LeapController.h"

LeapLib::LeapController::LeapController( LeapManager* leapManager )
{
	leapListener = new LeapLib::LeapListener( leapManager );
}

LeapLib::LeapController::~LeapController()
{
	if ( leapListener != NULL ) {
		delete ( leapListener );
	}
}

void LeapLib::LeapController::startListening()
{
	enableGesture( Leap::Gesture::TYPE_SWIPE );
	setPolicy( Leap::Controller::POLICY_IMAGES );
	addListener( *leapListener );
}

void LeapLib::LeapController::stopListening()
{
	removeListener( *leapListener );
}
