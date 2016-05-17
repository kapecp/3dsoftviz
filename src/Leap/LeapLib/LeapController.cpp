#include "LeapLib/LeapController.h"

Leap::LeapController::LeapController(LeapCameraManipulator* cameraManipulator){
	leapListener = new Leap::LeapListener(cameraManipulator);
}

Leap::LeapController::~LeapController(){
	if (leapListener != NULL){
		delete(leapListener);
	}
}

void Leap::LeapController::startListening()
{
	addListener( *leapListener );
}

void Leap::LeapController::stopListening()
{
	removeListener( *leapListener );
}
