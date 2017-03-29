#ifndef CUSTOMLEAPMANAGER_H
#define CUSTOMLEAPMANAGER_H

#include "Leap.h"
#include "LeapLib/LeapActions.h"

#include <osg/Group>
#include <osg/ref_ptr>
namespace Leap {
class LeapGestureHandler
{
public:
    LeapActions* leapActions;

    LeapGestureHandler( LeapManager* leapManager );
    ~LeapGestureHandler( void );
    void processGestures(Frame frame);
};
}
#endif
