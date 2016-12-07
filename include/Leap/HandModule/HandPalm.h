/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDPALM_H
#define HANDPALM_H

#include "Leap/HandModule/HandNode.h"
#include "Leap/HandModule/joint.h"
#include "Leap/HandModule/HandBone.h"
#include <list>
#include <osg/Group>

namespace Leap {
class HandPalm : public HandNode
{
public:
	HandPalm( float radius, osg::ref_ptr<osg::Group> handsGroup, int colorSwitch );
	void initStructure();
	void generateGeometry( float radius, int colorSwitch );

	osg::ref_ptr<osg::Group> fingerGroup;
	// 1 = GREEN JOINTS
	// 2 = BLUE JOINTS
	int colorSwitch;
	osg::ref_ptr<osg::Group> interFingerBoneGroup;
};
}
#endif // HANDPALM_H
