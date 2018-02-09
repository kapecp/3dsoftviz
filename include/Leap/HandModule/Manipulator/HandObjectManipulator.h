#ifndef HANDOBJECTMANIPULATOR_H
#define HANDOBJECTMANIPULATOR_H

#include "Leap/HandModule/Manipulator/HandMapper.h"
#include "Leap/HandModule/Model/HandPalm.h"
#include "osg/Camera"

#include "OpenCV/HandTracker.h"

#include <osg/Group>
#include <opencv2/core/core.hpp>

namespace Softviz {
namespace Leap {

class HandMapper;

class HandObjectManipulator
{

public:
	HandObjectManipulator( HandMapper* mapper, char upDirectionAxis );

	~HandObjectManipulator();

	/**
	 Method Used by CustomLeapManager class. Update position of palms and call methods updateFingers()
	 and updateInterFingerBones().
	 * @param leftHand - Leap representation of left hand (sensor output data).
	 * @param rightHand - Leap representation of right hand (sensor output data).
	 */
	void updateHands( ::Leap::Hand leftHand, ::Leap::Hand rightHand, HandPalm* leftPalm, HandPalm* rightPalm, osg::ref_ptr<osg::Camera> camera );

	/**
	 Method used in updateHands(). Iterates over fingers and for each call methods: updateJoints() and
	 updateFingerBones().
	 * @param palm - sphere representation of palm containing groups of figer joints and bones
	 * @param fingers - Leap representation of fingers (sensor output data).
	 */
	void updateFingers( HandPalm* palm, ::Leap::FingerList fingers, float diff );

	/**
	Method used in updateFingers(). Render joints of single finger.
	 * @param fingerGroup - Group cointaining sphere representation of joints of single finger
	 * @param finger - Leap representation of single finger (sensor output data).
	 * @param fingerPosition - Value representing index of position of finger (from THUMB to PINKY)
	 */
	void updateJoints( osg::Group*  fingerGroup, ::Leap::Finger finger, int fingerPosition, float diff );

	/**
	Method used in updateFingers(). Render bones of single finger.
	 * @param fingerGroup - Group cointaining cylinder representation of bones and of single finger
	 * @param finger - Leap representation of single finger (sensor output data).
	 * @param fingerPosition - Value representing index of position of finger (from THUMB to PINKY)
	 */
	void updateFingerBones( osg::Group*  fingerGroup, ::Leap::Finger finger, float diff );

	/**
	Method used in updateHands(). Render bones between fingers. Call method updateInterFingerWristBone().
	 * @param interFingerBoneGroup - Group cointaining cylinder representation of bones between fingers.
	 * @param fingers - Leap representation all fingers (sensor output data).
	 */
	void updateInterFingerBones( osg::Group*  interFingerBoneGroup, ::Leap::FingerList fingers, float diff );

	/**
	Method used in updateFingerBones(). Render wrist bone.
	 * @param interFingerBoneGroup - Group cointaining cylinder representation of bones between fingers.
	 * @param fingers - Leap representation all fingers (sensor output data).
	 */
	void updateInterFingerWristBone( osg::Group*  interFingerBoneGroup, ::Leap::FingerList fingers, float diff );

	::Leap::Vector changeHandUpDirectionAxis( ::Leap::Vector vector );

	void setHandMapper( HandMapper* mapper );

private:
	::Leap::Vector center;
	::Leap::Vector up;
	::Leap::Vector direction;
	HandMapper* mapper;
	char upDirectionAxis;
};

} // namespace Leap
} // namespace Softviz

#endif // HANDOBJECTMANIPULATOR_H
