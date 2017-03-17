#ifndef HANDTRACKERADAPTER_H
#define HANDTRACKERADAPTER_H

#include <osg/Group>
#include <osg/ref_ptr>
#include <opencv2/core/core.hpp>

namespace Leap {

class HandTrackerAdapter
{

public:

    HandTrackerAdapter();

    ~HandTrackerAdapter();

    // uses opencv to find hand in image buffer
    void trackHands( unsigned char* buffer,float depth);

};
}

#endif // HANDTRACKERADAPTER_H
