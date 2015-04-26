#ifndef FINGERPOSITIONDETECTOR_H
#define FINGERPOSITIONDETECTOR_H

#include "Leap.h"

namespace Leap {

/**
 * @brief The FingerPositionDetector class
 * Contains fuctions that define the position of fingers using leap sensor
 */
class FingerPositionDetector
{
public:
	/**
	 * @brief isHandExtended
	 * @param hand - Hand object from Leap sensor
	 * @return true if the hand has all fingers extended.
	 */
	static bool isHandExtended( Hand hand );
private:
	/**
	 * @brief getNumberOfExtendedFingers
	 * @param fingers - FingerList object obtained from Hand object
	 * @return the number of extended fingers in the range of 0-5
	 */
	static int getNumberOfExtendedFingers( FingerList fingers );

};

}

#endif // FINGERPOSITIONDETECTOR_H
