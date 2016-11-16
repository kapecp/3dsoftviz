/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDPALM_H
#define HANDPALM_H

#include "Leap/HandModule/HandNode.h"
#include "Leap/HandModule/Joint.h"
#include <list>

namespace Leap {
class HandPalm : public HandNode {
public:
    HandPalm(float radius);
    void initStructure();

    std::list<Joint*> coreJoints;
};
}
#endif // HANDPALM_H
