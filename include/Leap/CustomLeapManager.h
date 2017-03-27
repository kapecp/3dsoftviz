#ifndef CUSTOMLEAPMANAGER_H
#define CUSTOMLEAPMANAGER_H

#include "Leap.h"
#include "LeapLib/LeapManager.h"

#include <osg/Group>
#include <osg/ref_ptr>

namespace Vwr {
class CameraManipulator;
class CoreGraph;
}

namespace Layout {
class LayoutThread;
}

namespace Leap {

class HandPalm;

class CustomLeapManager : public LeapManager
{
public:
	CustomLeapManager( Vwr::CameraManipulator* cameraManipulator,Layout::LayoutThread* layout,
					   Vwr::CoreGraph* coreGraph, osg::ref_ptr<osg::Group> handsGroup = nullptr );
	~CustomLeapManager( void );

	void enableCameraMovement( Movement direction );
	void disableCameraMovement();
	void rotateCamera( float py0, float px0, double throwScale, float py1, float px1 );

    Leap::Vector recalculateDepthNode(Leap::Vector vector, float diff);

	/**
	 Method Used by LeapLib/LeapActions class. Update position of palms and call methods updateFingers()
	 and updateInterFingerBones().
	 * @param leftHand - Leap representation of left hand (sensor output data).
	 * @param rightHand - Leap representation of right hand (sensor output data).
	 */
	void updateHands( Leap::Hand leftHand, Leap::Hand rightHand );

	/**
	 Method used in updateHands(). Iterates over fingers and for each call methods: updateJoints() and
	 updateFingerBones().
	 * @param palm - sphere representation of palm containing groups of figer joints and bones
	 * @param fingers - Leap representation of fingers (sensor output data).
	 */
    void updateFingers( HandPalm* palm, Leap::FingerList fingers, float diff);

	/**
	Method used in updateFingers(). Render joints of single finger.
	 * @param fingerGroup - Group cointaining sphere representation of joints of single finger
	 * @param finger - Leap representation of single finger (sensor output data).
	 * @param fingerPosition - Value representing index of position of finger (from THUMB to PINKY)
	 */
    void updateJoints( osg::Group*  fingerGroup, Leap::Finger finger, int fingerPosition, float diff );

	/**
	Method used in updateFingers(). Render bones of single finger.
	 * @param fingerGroup - Group cointaining cylinder representation of bones and of single finger
	 * @param finger - Leap representation of single finger (sensor output data).
	 * @param fingerPosition - Value representing index of position of finger (from THUMB to PINKY)
	 */
    void updateFingerBones( osg::Group*  fingerGroup, Leap::Finger finger, float diff );

	/**
	Method used in updateHands(). Render bones between fingers. Call method updateInterFingerWristBone().
	 * @param interFingerBoneGroup - Group cointaining cylinder representation of bones between fingers.
	 * @param fingers - Leap representation all fingers (sensor output data).
	 */
    void updateInterFingerBones( osg::Group*  interFingerBoneGroup, Leap::FingerList fingers, float diff );

	/**
	Method used in updateFingerBones(). Render wrist bone.
	 * @param interFingerBoneGroup - Group cointaining cylinder representation of bones between fingers.
	 * @param fingers - Leap representation all fingers (sensor output data).
	 */
    void updateInterFingerWristBone( osg::Group*  interFingerBoneGroup, Leap::FingerList fingers, float diff );

	//berger
    int updateCoreGraphBackground( const unsigned char* buffer, float depth );

    float getHandsDepthInformation(float leftHandPosition, float rightHandPosition);

	//jurik
	void graphRotateSwipe( int swipeDirection );
	void scaleNodes( bool scaleUp );
	void rotateArucoLeft();
	void rotateArucoRight();
	void scaleEdgesUp();
	void scaleEdgesDown();
	//*****

	Vwr::CameraManipulator* cameraManipulator;
	Layout::LayoutThread* layout;
	Vwr::CoreGraph* coreGraph;

	osg::ref_ptr<osg::Group> handsGroup;
	osg::Vec3d eye;
	osg::Vec3d center;
	osg::Vec3d up;
	osg::Vec3d direction;
};
}
#endif
