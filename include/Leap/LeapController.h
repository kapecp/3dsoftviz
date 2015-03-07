#ifndef LEAPCONTROLLER_H
#define LEAPCONTROLLER_H

#include "Leap.h"
#include "Leap/LeapListener.h"

namespace Leap {

class LeapController;

/**
 * @author Filip Mikle
 * @brief The Leap controller class
 */
class LeapController:Controller
{
  public:
    LeapListener leapListener;
    void startListening();
    void stopListening();
};
}

#endif // LEAPCONTROLLER_H
