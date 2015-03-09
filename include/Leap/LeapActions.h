#ifndef LEAPACTIONS_H
#define LEAPACTIONS_H
#include "Leap.h"
#include "math.h"

namespace Leap {

class LeapActions;

/**
 * @author Filip Mikle
 * @brief The Leap Action class
 * @brief All actions that will be executed on the leap sensor should be defined here and used in LeapListener
 */
class LeapActions
{
  public:
    // These methods should not have names onSwipe, onXXX. They are placeholders for further functionality.
    // The names should correspond to the functionality provided by the method i.e. moveGraph();
    void Leap::LeapActions::onSwipe(Gesture gesture);
    void Leap::LeapActions::onCircle(Gesture gesture);
    void Leap::LeapActions::onKeyTap(Gesture gesture);
    void Leap::LeapActions::onScreenTap(Gesture gesture);
};
}
#endif // LEAPACTIONS_H
