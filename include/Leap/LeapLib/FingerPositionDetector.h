#ifndef FINGERPOSITIONDETECTOR_H
#define FINGERPOSITIONDETECTOR_H
#include "LeapLib/LeapExport.h"

#include "Leap.h"

namespace Leap {

/**
 * @brief The FingerPositionDetector class
 * Contains fuctions that define the position of fingers using leap sensor
 */
class LEAPLIB_EXPORT FingerPositionDetector
{
public:
	/**
	 * @brief isHandExtended
	 * @param hand - Hand object from Leap sensor
	 * @return true if the hand has all fingers extended.
	 */
	static bool isHandExtended( Hand hand );
};

}

#endif // FINGERPOSITIONDETECTOR_H
