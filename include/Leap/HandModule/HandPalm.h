/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDPALM_H
#define HANDPALM_H

#include "Leap/HandModule/HandNode.h"
#include "Leap/HandModule/Joint.h"
#include "Leap/HandModule/HandBone.h"
#include <list>
#include <osg/Group>

namespace Leap {
class HandPalm : public HandNode
{
public:
	/**
	  * Constructor of HandPalm. Automatically adds itself, fingerGroups and interFingerBoneGroup to the given osg::Group,
	  * calls initStructure() and generateGeometry().
	  *
	  * @param radius - Value representing palm's position in the finger.
	  * @param handsGroup - Group of hands containing both palms and it's fingers.
	  * @param colorSwitch - Value representing colour of palm (differs between hands)
	 */
	HandPalm( float radius, osg::ref_ptr<osg::Group> handsGroup, int colorSwitch );
	/**
	 * Starts creation of joint hierarchy for fingers and instatiates bones.
	*/
	void initStructure();
	/**
	 * Method used in constructor to generate hierarchy of graph objects to visualize a palm.
	 * Geode -> ShapeDrawable -> Sphere
	 *
	 * @param radius - Radius of sphere drawable in geode.
	 * @param colorSwitch - Value representing colour of palm.
	*/
	void generateGeometry( float radius, int colorSwitch );

	osg::ref_ptr<osg::Group> fingerGroup;
	// 1 = GREEN JOINTS
	// 2 = BLUE JOINTS
	int colorSwitch;
	osg::ref_ptr<osg::Group> interFingerBoneGroup;
};
}
#endif // HANDPALM_H
