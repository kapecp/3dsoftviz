/**
 * Created by Patrik Berger on 19.11.2016.
 */
#ifndef BONE_H
#define BONE_H

#include "Leap/HandModule/HandNode.h"
#include "Leap/HandModule/Joint.h"

namespace Leap {
class HandBone : public HandNode
{
public:

	/**
	  * @brief CYLINDER_RADIUS - hrubka kosti
	  */
	const float CYLINDER_RADIUS  = 0.03f;

	/**
	 * @brief HEIGHT - dlzka kosti
	 */
	const float HEIGHT = 0.2f;

	/**
	 * Constructor of HandBone. Automatically adds itself to the given osg::Group, calls
	 * generateGeometry().
	 *
	 * @param type - Value representing bone's type (position in the finger).
	 * @param boneGroup - Group of bones representing a single finger.
	*/
	HandBone( int type, osg::ref_ptr<osg::Group> boneGroup );

	/**
	  * Method used in constructor to generate hierarchy of graph objects to visualize a single bone.
	  * Geode -> ShapeDrawable -> Sphere
	  *
	  * @param radius - Radius of cylinder drawable in geode.
	  * @param colorSwitch - Value representing colour of bone - not used in current implementation.
	 */
	void generateGeometry( float radius, int colorSwitch );

	osg::ref_ptr<osg::Group> boneGroup;

private:
	int type;
};
}

#endif // BONE_H
