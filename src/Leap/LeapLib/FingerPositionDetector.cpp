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
