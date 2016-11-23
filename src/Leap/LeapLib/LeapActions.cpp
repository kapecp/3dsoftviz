#include <easylogging++.h>

#include "LeapLib/LeapActions.h"
#include "Leap/LeapSleeper.h"

Leap::LeapActions::LeapActions( LeapManager* leapManager )
{
	this->leapManager = leapManager;
	isCameraMoving = true;
	LOG( INFO ) << "LeapActions() Constructor";
}

Leap::LeapActions::~LeapActions()
{
	if ( leapManager != NULL ) {
		delete( leapManager );
	}
	LOG( INFO ) << "~LeapActions() Destructor";
}

void Leap::LeapActions::moveCamera( Leap::Gesture gesture )
{
	// swipe gesture
	SwipeGesture swipe = gesture;
	Vector direction = swipe.direction();
	float gestureDuration = static_cast<float>( gesture.duration() )/1000.0f;

	//skip zero movement gesture
	//if ( static_cast<int>(direction[0]) == 0.0f && static_cast<int>(direction[1]) == 0.0f && static_cast<int>(direction[2]) == 0.0f ) {
	if ( static_cast<int>( direction[0] ) == 0 && static_cast<int>( direction[1] ) == 0 && static_cast<int>( direction[2] ) == 0 ) {
		return;
	}

	float absDir0 = std::abs( direction[0] );
	float absDir1 = std::abs( direction[1] );

	// compare direction[0] and direction[1] to find out what kind of movement are we working with
	// >= instead of > to avoid edge case, no camera movement would happen with absDir0 equal to absDir1
	if ( absDir0 >= absDir1 ) { // horizontal movement
		if ( direction[0] > 0 ) {
			LOG( INFO ) << "SwipeGesture - right";
			leapManager->enableCameraMovement( Leap::LeapManager::Movement::RIGHT );
		}
		else if ( direction[0] < 0 ) {
			LOG( INFO ) << "SwipeGesture - left";
			leapManager->enableCameraMovement( Leap::LeapManager::Movement::LEFT );
		}
	}
	else if ( absDir0 < absDir1 ) { // vertical movement
		if ( direction[1] > 0 ) {
			LOG( INFO ) << "SwipeGesture - down";
			leapManager->enableCameraMovement( Leap::LeapManager::Movement::DOWN );
		}
		else if ( direction[1] < 0 ) {
			LOG( INFO ) << "SwipeGesture - up";
			leapManager->enableCameraMovement( Leap::LeapManager::Movement::UP );
		}
	}
	else {
		if ( direction[2] > 0 ) {
			LOG( INFO ) << "SwipeGesture - backward";
			leapManager->enableCameraMovement( Leap::LeapManager::Movement::BACKWARD );
		}
		else if ( direction[2] < 0 ) {
			LOG( INFO ) << "SwipeGesture - forward";
			leapManager->enableCameraMovement( Leap::LeapManager::Movement::FORWARD );
		}
	}
	//if we get here, the camera is moving
	LeapSleeper::sleep( static_cast<unsigned long >( gestureDuration ) );
	leapManager->disableCameraMovement();
}

void Leap::LeapActions::zoomGraph( Leap::Gesture gesture )
{
	CircleGesture circle = gesture;

	if ( gesture.state() == Leap::Gesture::STATE_START ) {
		if ( circle.pointable().direction().angleTo( circle.normal() ) <= PI/2 ) {
			LOG( INFO ) << "[onFrame()::CircleGesture - clockwise start]";
			leapManager->enableCameraMovement( Leap::LeapManager::Movement::FORWARD );
		}
		else {
			LOG( INFO ) << "[onFrame()::CircleGesture - counterclockwise start]";
			leapManager->enableCameraMovement( Leap::LeapManager::Movement::BACKWARD );
		}
	}
	else if ( gesture.state() == Leap::Gesture::STATE_STOP ) {
		LOG( INFO ) << "[onFrame()::CircleGesture - stop]";
		leapManager->disableCameraMovement();
	}
}

void Leap::LeapActions::onKeyTap( Leap::Gesture gesture )
{
	isCameraMoving = !isCameraMoving;
	LOG( INFO ) << "KeyTapGesture";
}
/*
void LeapActions::onScreenTap( Leap::Gesture gesture )
{
	// screen tap gesture
	LOG(INFO) << "ScreenTapGesture";
}
*/
void Leap::LeapActions::rotateGraph( Leap::Gesture gesture )
{

	// swipe gesture
	SwipeGesture swipe = gesture;
	Vector direction = swipe.direction();

	// >= instead of > to avoid edge case, no rotation would happen with abs direction[0] equal to abs direction[1]
	if ( std::abs( direction[0] ) >= std::abs( direction[1] ) ) { // horizontal movement
		if ( direction[0] > 0 ) {
			leapManager->rotateCamera( 0, 0, 1, 0, 0.05f );
		}
		else if ( direction[0] < 0 ) {
			leapManager->rotateCamera( 0, 0, 1, 0, -0.05f );
		}
	}
	else { // vertical movement
		if ( direction[1] > 0 ) {
			leapManager->rotateCamera( 0, 0 ,1, 0.05f, 0 );
		}
		else if ( direction[1] < 0 ) {
			leapManager->rotateCamera( 0, 0, 1, -0.05f, 0 );
		}
	}
}

void Leap::LeapActions::changeViewAngle( Leap::DirectionDetector::Direction direction )
{

	switch ( direction ) {
		case Leap::DirectionDetector::Direction::LEFT :
			leapManager->rotateCamera( 0, 0, 1, 0, -0.01f );
			break;
		case Leap::DirectionDetector::Direction::RIGHT :
			leapManager->rotateCamera( 0, 0, 1, 0, 0.01f );
			break;
		case Leap::DirectionDetector::Direction::UP :
			leapManager->rotateCamera( 0, 0, 1, -0.01f, 0 );
			break;
		case Leap::DirectionDetector::Direction::DOWN :
			leapManager->rotateCamera( 0, 0, 1, 0.01f, 0 );
			break;
		case Leap::DirectionDetector::Direction::STEADY :
			// stuff
			break;
		default :
			break;
	}

}

void Leap::LeapActions::startMovingForward()
{
	leapManager->enableCameraMovement( Leap::LeapManager::Movement::FORWARD );
}

void Leap::LeapActions::stopMovingForward()
{
	leapManager->disableCameraMovement();
}

//jurik
void Leap::LeapActions::graphRotateSwipe( int swipeDirection )
{

	leapManager->graphRotateSwipe( swipeDirection );
}

void Leap::LeapActions::rotateAruco( Leap::DirectionDetector::Direction direction )
{

	switch ( direction ) {
		case Leap::DirectionDetector::Direction::LEFT :
			leapManager->rotateArucoLeft();
			break;
		case Leap::DirectionDetector::Direction::RIGHT :
			leapManager->rotateArucoRight();
			break;
		case Leap::DirectionDetector::Direction::STEADY :
			break;
		case Leap::DirectionDetector::Direction::UP :
			break;
		case Leap::DirectionDetector::Direction::DOWN :
			break;
		default :
			break;
	}
}

void Leap::LeapActions::scaleEdges( Leap::DirectionDetector::Direction direction )
{
	switch ( direction ) {
		case Leap::DirectionDetector::Direction::LEFT :
			leapManager->scaleEdgesUp();
			break;
		case Leap::DirectionDetector::Direction::RIGHT :
			leapManager->scaleEdgesDown();
			break;
		case Leap::DirectionDetector::Direction::STEADY :
			break;
		case Leap::DirectionDetector::Direction::UP :
			break;
		case Leap::DirectionDetector::Direction::DOWN :
			break;
		default :
			break;
	}
}
void Leap::LeapActions::scaleNodes( bool scaleUp )
{
	leapManager->scaleNodes( scaleUp );
}

void Leap::LeapActions::updateARHands( Leap::Hand leftHand, Leap::Hand rightHand )
{
	leapManager->updateHands( leftHand, rightHand );
}
