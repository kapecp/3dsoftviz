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

    // converts buffer data to its cv::Mat representation
    cv::Mat convertBuffer( unsigned char* buffer );

};
}

#endif // HANDTRACKERADAPTER_H
