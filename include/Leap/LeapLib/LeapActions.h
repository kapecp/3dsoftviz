#ifndef LEAPACTIONS_H
#define LEAPACTIONS_H
#include "Leap.h"
#include "math.h"
#include "LeapLib/DirectionDetector.h"
#include "LeapLib/LeapCameraManipulator.h"
#include "LeapLib/LeapExport.h"

namespace Leap {

/**
 * @author Filip Mikle
 * @brief The Leap Action class
 * @brief All actions that will be executed on the leap sensor should be defined here and used in LeapListener
 */
class LEAPLIB_EXPORT LeapActions
{

public:
	/**
	 * @brief Initializes MouseControl and sets zoomCounter to 0
	 */
	LeapActions(LeapCameraManipulator* cameraManipulator);
	~LeapActions();

	/**
	 * @brief Zooms the graph in or out depending on the hand movement(Circle gesture)
	 * @brief Clockwise - zooming out, Counterclockwise - zooming in
	 * @param gesture - CircleGesture to be used
	 */
	void zoomGraph( Gesture gesture );

	/**
	 * @brief Moves the graph using swipe gesture. Possible directions are UP, DOWN, LEFT, RIGHT
	 * @param gesture - SwipeGesture to be used
	 */
	void moveCamera( Gesture gesture );

	/**
	 * @brief Leap::LeapActions::rotateGraph
	 * @param gesture
	 */
	void rotateGraph( Gesture gesture );

	// These methods should not have names onSwipe, onXXX. They are placeholders for further functionality.
	// The names should correspond to the functionality provided by the method i.e. moveGraph();
	void onKeyTap( Gesture gesture );
	//void onScreenTap( Gesture::gesture );

	/**
	 * @brief Leap::LeapActions::changeViewAngle
	 * Change angle of camera in flymode using leap sensor
	 * @param direction - Direction enum from DirectionDetector.
	 */
	void changeViewAngle( DirectionDetector::Direction direction );
	/**
	 * @brief Leap::LeapActions::startMovingForward
	 * Starts the movement of camera forward.
	 */
	void startMovingForward();
	/**
	 * @brief Leap::LeapActions::stopMovingForward
	 * Stops forward movement of camera.
	 */
	void stopMovingForward();

	/**
	 * @brief isCameraMoving is variable to decide whether to move camera or rotate graph
	 */
	bool isCameraMoving;

	LeapCameraManipulator* cameraManipulator;
};
}


#endif // LEAPACTIONS_H
