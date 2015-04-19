#ifndef FINGERPOSITIONDETECTOR_H
#define FINGERPOSITIONDETECTOR_H

#include "Leap.h"

namespace Leap {

class FingerPositionDetector
{
public:

    static int getNumberOfExtendedFingers(FingerList fingers);
    static bool isHandExtended(Hand hand);

};

}

#endif // FINGERPOSITIONDETECTOR_H
