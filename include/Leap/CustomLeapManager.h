
#ifndef CUSTOMLEAPMANAGER_H
#define CUSTOMLEAPMANAGER_H

#include "HandModule/Manipulator/HandObjectManipulator.h"
#include "LeapLib/LeapManager.h"

#include "Leap.h"

#include <osg/Group>
#include <osg/ref_ptr>

namespace Vwr {
class CameraManipulator;
class CoreGraph;
}

namespace Layout {
class LayoutThread;
}

namespace Softviz {
namespace Leap {

class HandPalm;
class HandObjectManipulator;

class CustomLeapManager : public LeapLib::LeapManager
{
public:
	CustomLeapManager( Vwr::CameraManipulator* cameraManipulator,Layout::LayoutThread* layout,
					   Vwr::CoreGraph* coreGraph, osg::ref_ptr<osg::Group> handsGroup = nullptr );
	~CustomLeapManager( void );

	void enableCameraMovement( Movement direction );
	void disableCameraMovement();
	void rotateCamera( float py0, float px0, double throwScale, float py1, float px1 );

	/**
	 Method Used by LeapLib/LeapActions class. Update position of palms and call methods updateFingers()
	 and updateInterFingerBones().
	 * @param leftHand - Leap representation of left hand (sensor output data).
	 * @param rightHand - Leap representation of right hand (sensor output data).
	 */
	void updateHands( ::Leap::Hand leftHand, ::Leap::Hand rightHand );
	void updateHands( QDataStream* stream );

	//berger
	/**
	 * @brief updateCoreGraphBackground - send stream from leap to background image
	 * @param buffer - image
	 * @param depth - not used anymore
	 * @return
	 */
	int updateCoreGraphBackground( const unsigned char* buffer, float depth );

	//jurik
	void graphRotateSwipe( int swipeDirection );
	void scaleNodes( bool scaleUp );
	void rotateArucoLeft();
	void rotateArucoRight();
	void scaleEdgesUp();
	void scaleEdgesDown();
	void scaleGraph( bool scaleUp );

	void selectNode( bool right );
	//*****


	Vwr::CameraManipulator* cameraManipulator;
	Layout::LayoutThread* layout;
	Vwr::CoreGraph* coreGraph;
	/**
	 * @brief handObjectManipulator - object for updating hand model position
	 */
	HandObjectManipulator* handObjectManipulator;
	/**
	 * @brief handsGroup - hands nodes and bones hierachy
	 */
	osg::ref_ptr<osg::Group> handsGroup;
};

} // namespace Leap
} // namespace Softviz

#endif // CUSTOMLEAPMANAGER_H

