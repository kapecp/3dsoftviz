#include "LeapLib/FingerPositionDetector.h"

bool Leap::FingerPositionDetector::isHandExtended( Hand hand )
{
	FingerList fingers = hand.fingers();

	for ( int i=0; i < fingers.count(); ++i ) {
		if ( !fingers[i].isExtended() ) {
			return false;
		}
	}

	return true;
}

bool Leap::FingerPositionDetector::isIndexFingerExtended( Hand hand )
{
	FingerList fingers = hand.fingers();

	if ( fingers[0].isExtended() ) {
		return false;
	}

	if ( !fingers[1].isExtended() ) {
		return false;
	}

	for ( int i=2; i < fingers.count(); ++i ) {
		if ( fingers[i].isExtended() ) {
			return false;
		}
	}

	return true;
}

