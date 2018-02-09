#include "LeapLib/FingerPositionDetector.h"

bool LeapLib::FingerPositionDetector::isHandExtended( Leap::Hand hand )
{
	Leap::FingerList fingers = hand.fingers();

	for ( int i=0; i < fingers.count(); ++i ) {
		if ( !fingers[i].isExtended() ) {
			return false;
		}
	}

	return true;
}

bool LeapLib::FingerPositionDetector::isIndexFingerExtended( Leap::Hand hand )
{
	Leap::FingerList fingers = hand.fingers();

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

