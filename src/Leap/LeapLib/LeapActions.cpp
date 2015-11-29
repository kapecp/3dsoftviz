#include "LeapLib/LeapActions.h"
#include <QDebug>
#include <QtCore/qt_windows.h>	//sleep include, TODO

Leap::LeapActions::LeapActions(LeapCameraManipulator* cameraManipulator)
{
	this->cameraManipulator = cameraManipulator;
	isCameraMoving = true;
	qDebug() << "LeapActions() Constructor";
}

Leap::LeapActions::~LeapActions()
{
	if (cameraManipulator != NULL){
		delete(cameraManipulator);
	}
	qDebug() << "~LeapActions() Destructor";
}

void Leap::LeapActions::moveCamera( Leap::Gesture gesture )
{
	// swipe gesture
	SwipeGesture swipe = gesture;
	Vector direction = swipe.direction();
	float gestureDuration = static_cast<float>( gesture.duration() )/1000.0f;

	//skip zero movement gesture
	if ( direction[0] == 0.0f && direction[1] == 0.0f && direction[2] == 0.0f ) {
		return;
	}

	float absDir0 = abs( direction[0] );
	float absDir1 = abs( direction[1] );

	// compare direction[0] and direction[1] to find out what kind of movement are we working with
	// >= instead of > to avoid edge case, no camera movement would happen with absDir0 equal to absDir1
	if ( absDir0 >= absDir1 ) { // horizontal movement
		if ( direction[0] > 0 ) {
			qDebug() << "SwipeGesture - right";
			cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::RIGHT );
		}
		else if ( direction[0] < 0 ) {
			qDebug() << "SwipeGesture - left";
			cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::LEFT );
		}
	}
	else if ( absDir0 < absDir1 ) { // vertical movement
		if ( direction[1] > 0 ) {
			qDebug() << "SwipeGesture - down";
			cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::DOWN );
		}
		else if ( direction[1] < 0 ) {
			qDebug() << "SwipeGesture - up";
			cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::UP );
		}
	}
	else {
		if ( direction[2] > 0 ) {
			qDebug() << "SwipeGesture - backward";
			cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::BACKWARD );
		}
		else if ( direction[2] < 0 ) {
			qDebug() << "SwipeGesture - forward";
			cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::FORWARD );
		}
	}
	//if we get here, the camera is moving
	Sleep( ( DWORD )gestureDuration );
	cameraManipulator->disableCameraMovement();
}

void Leap::LeapActions::zoomGraph( Leap::Gesture gesture )
{
	CircleGesture circle = gesture;

	if ( gesture.state() == Leap::Gesture::STATE_START ) {
		if ( circle.pointable().direction().angleTo( circle.normal() ) <= PI/2 ) {
			qDebug() << "[onFrame()::CircleGesture - clockwise start]";
			cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::FORWARD );
		}
		else {
			qDebug() << "[onFrame()::CircleGesture - counterclockwise start]";
			cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::BACKWARD );
		}
	}
	else if ( gesture.state() == Leap::Gesture::STATE_STOP ) {
		qDebug() << "[onFrame()::CircleGesture - stop]";
		cameraManipulator->disableCameraMovement();
	}
}

void Leap::LeapActions::onKeyTap( Leap::Gesture gesture )
{
	isCameraMoving = !isCameraMoving;
	qDebug() << "KeyTapGesture";
}
/*
void LeapActions::onScreenTap( Leap::Gesture gesture )
{
    // screen tap gesture
    qDebug() << "ScreenTapGesture";
}
*/
void Leap::LeapActions::rotateGraph( Leap::Gesture gesture )
{

	// swipe gesture
	SwipeGesture swipe = gesture;
	Vector direction = swipe.direction();

	// >= instead of > to avoid edge case, no rotation would happen with abs direction[0] equal to abs direction[1]
	if ( abs( direction[0] ) >= abs( direction[1] ) ) { // horizontal movement
		if ( direction[0] > 0 ) {
			cameraManipulator->rotateCamera( 0, 0, 1, 0, 0.05f );
		}
		else if ( direction[0] < 0 ) {
			cameraManipulator->rotateCamera( 0, 0, 1, 0, -0.05f );
		}
	}
	else { // vertical movement
		if ( direction[1] > 0 ) {
			cameraManipulator->rotateCamera( 0, 0 ,1, 0.05f, 0 );
		}
		else if ( direction[1] < 0 ) {
			cameraManipulator->rotateCamera( 0, 0, 1, -0.05f, 0 );
		}
	}
}

void Leap::LeapActions::changeViewAngle( Leap::DirectionDetector::Direction direction )
{

	switch ( direction ) {
		case Leap::DirectionDetector::Direction::LEFT :
			cameraManipulator->rotateCamera( 0, 0, 1, 0, -0.01f );
			break;
		case Leap::DirectionDetector::Direction::RIGHT :
			cameraManipulator->rotateCamera( 0, 0, 1, 0, 0.01f );
			break;
		case Leap::DirectionDetector::Direction::UP :
			cameraManipulator->rotateCamera( 0, 0, 1, -0.01f, 0 );
			break;
		case Leap::DirectionDetector::Direction::DOWN :
			cameraManipulator->rotateCamera( 0, 0, 1, 0.01f, 0 );
			break;
		case Leap::DirectionDetector::Direction::STEADY :
			// stuff
			break;
	}

}

void Leap::LeapActions::startMovingForward()
{
	cameraManipulator->enableCameraMovement( Leap::LeapCameraManipulator::Movement::FORWARD );
}

void Leap::LeapActions::stopMovingForward()
{
	cameraManipulator->disableCameraMovement();
}
