/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDPALM_H
#define HANDPALM_H

#include "Leap/HandModule/HandNode.h"
#include "Leap/HandModule/Joint.h"
#include <list>
#include <osg/Group>

namespace Leap {
class HandPalm : public HandNode {
public:
    HandPalm(float radius, osg::ref_ptr<osg::Group> handsGroup );
    void initStructure();

    osg::ref_ptr<osg::Group> fingerGroup;
};
}
#endif // HANDPALM_H
