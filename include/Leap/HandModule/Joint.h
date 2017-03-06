/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef JOINT_H
#define JOINT_H

#include "Leap/HandModule/HandNode.h"
namespace Leap {
class Joint : public HandNode
{
public:
	/**
	 * Constructor of joint. Automatically adds itself to the given osg::Group, calls
	 * initStructure() and generateGeometry().
	 *
	 * @param level - Value representing joint's position in the finger.
	 * @param position - Value representing the finger of joint (eg. middle, ring).
	 * @param fingerJointGroup - Group of joints representing a single finger.
	 * @param colorSwitch - Value representing colour of joint (differs between hands)
	*/
	Joint( int level, int position, osg::ref_ptr<osg::Group> fingerJointGroup, int colorSwitch );
	/**
	 * Method used in constructor to generate hierarchy of joints.
	 * Creates another Joint and saves it as nextJoint
	*/
	void initStructure();
	/**
	 * Method used in constructor to generate hierarchy of graph objects to visualize a single joint.
	 * Geode -> ShapeDrawable -> Sphere
	 *
	 * @param radius - Radius of sphere drawable in geode.
	 * @param colorSwitch - Value representing colour of joint.
	*/
	void generateGeometry( float radius, int colorSwitch );

	int colorSwitch;
	Joint* nextJoint;
	osg::ref_ptr<osg::Group> fingerJointGroup;
private:
	// 0-3 Value representing joint's position in the finger.
	int level;
	// 0-5 Value representing the finger of joint (eg. middle, ring).
	int position;
};
}
#endif // JOINT_H
