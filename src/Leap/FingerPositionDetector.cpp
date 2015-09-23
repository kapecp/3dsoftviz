#include "Leap/FingerPositionDetector.h"


int Leap::FingerPositionDetector::getNumberOfExtendedFingers( FingerList fingers )
{
	int numberOfExtendedFingers = 0;

	for ( int i=0; i < fingers.count(); i++ ) {
		if ( fingers[i].isExtended() ) {
			numberOfExtendedFingers++;
		}
	}

	return numberOfExtendedFingers;
}

bool Leap::FingerPositionDetector::isHandExtended( Hand hand )
{
	FingerList fingers = hand.fingers();
	int numberOfExtendedFingers = getNumberOfExtendedFingers( fingers );

	if ( numberOfExtendedFingers == 5 ) {
		return true;
	}

	return false;
}
