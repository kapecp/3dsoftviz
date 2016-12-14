#ifndef CUSTOMLEAPMANAGER_H
#define CUSTOMLEAPMANAGER_H

#include "Leap.h"
#include "LeapLib/LeapManager.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/CoreGraph.h"
#include "Layout/LayoutThread.h"
#include "Layout/FRAlgorithm.h"
#include "HandModule/HandPalm.h"
#include <osg/ref_ptr>
#include <osg/Transform>
#include <osg/Matrix>
#include <osg/MatrixTransform>

namespace Leap {

class CustomLeapManager : public LeapManager
{
public:
	CustomLeapManager( Vwr::CameraManipulator* cameraManipulator,Layout::LayoutThread* layout,
					   Vwr::CoreGraph* coreGraph, osg::ref_ptr<osg::Group> handsGroup = NULL );
	~CustomLeapManager( void );

	void enableCameraMovement( Movement direction );
	void disableCameraMovement();
	void rotateCamera( float py0, float px0, double throwScale, float py1, float px1 );
	void updateHands( Leap::Hand leftHand, Leap::Hand rightHand );
	void updateFingers( HandPalm* palm, Leap::FingerList fingers );
	void updateJoints( osg::Group*  fingerGroup, Leap::Finger finger, int fingerPosition );
	void updateFingerBones( osg::Group*  fingerGroup, Leap::Finger finger );
	void updateInterFingerBones( osg::Group*  interFingerBoneGroup, Leap::FingerList fingers );
	void updateInterFingerWristBone( osg::Group*  interFingerBoneGroup, Leap::FingerList fingers );

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


};
}
#endif
