#include <easylogging++.h>

#include "LeapLib/LeapActions.h"
#include "Leap/LeapSleeper.h"

LeapLib::LeapActions::LeapActions( LeapManager* leapManager )
{
	this->leapManager = leapManager;
	isCameraMoving = true;
	LOG( INFO ) << "LeapActions() Constructor";
}

LeapLib::LeapActions::~LeapActions()
{
	if ( leapManager != NULL ) {
		delete ( leapManager );
	}
	LOG( INFO ) << "~LeapActions() Destructor";
}

void LeapLib::LeapActions::moveCamera( Leap::Gesture gesture )
{
	// swipe gesture
	Leap::SwipeGesture swipe = gesture;
	Leap::Vector direction = swipe.direction();
	float gestureDuration = static_cast<float>( gesture.duration() )/1000.0f;
	LOG( INFO ) << direction;
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
			leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::RIGHT );
		}
		else if ( direction[0] < 0 ) {
			LOG( INFO ) << "SwipeGesture - left";
			leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::LEFT );
		}
	}
	else if ( absDir0 < absDir1 ) { // vertical movement
		if ( direction[1] > 0 ) {
			LOG( INFO ) << "SwipeGesture - down";
			leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::DOWN );
		}
		else if ( direction[1] < 0 ) {
			LOG( INFO ) << "SwipeGesture - up";
			leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::UP );
		}
	}
	else {
		if ( direction[2] > 0 ) {
			LOG( INFO ) << "SwipeGesture - backward";
			leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::BACKWARD );
		}
		else if ( direction[2] < 0 ) {
			LOG( INFO ) << "SwipeGesture - forward";
			leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::FORWARD );
		}
	}
	//if we get here, the camera is moving
	LeapSleeper::sleep( static_cast<unsigned long >( gestureDuration ) );
	leapManager->disableCameraMovement();
}

void LeapLib::LeapActions::zoomGraph( Leap::Gesture gesture )
{
	Leap::CircleGesture circle = gesture;

	if ( gesture.state() == Leap::Gesture::STATE_START ) {
		if ( circle.pointable().direction().angleTo( circle.normal() ) <= Leap::PI/2 ) {
			LOG( INFO ) << "[onFrame()::CircleGesture - clockwise start]";
			leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::FORWARD );
		}
		else {
			LOG( INFO ) << "[onFrame()::CircleGesture - counterclockwise start]";
			leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::BACKWARD );
		}
	}
	else if ( gesture.state() == Leap::Gesture::STATE_STOP ) {
		LOG( INFO ) << "[onFrame()::CircleGesture - stop]";
		leapManager->disableCameraMovement();
	}
}

void LeapLib::LeapActions::onKeyTap( Leap::Gesture gesture )
{
	isCameraMoving = !isCameraMoving;
	LOG( INFO ) << "KeyTapGesture";
}

void LeapLib::LeapActions::onScreenTap( Leap::Gesture gesture )
{
	// screen tap gesture
	LOG( INFO ) << "ScreenTapGesture";
}

void LeapLib::LeapActions::rotateGraph( Leap::Gesture gesture )
{

	// swipe gesture
	Leap::SwipeGesture swipe = gesture;
	Leap::Vector direction = swipe.direction();

	LOG( INFO ) << "[LeapActions::rotateGraph] LeapVektor direction[x,y,z]: [" +
				std::to_string( direction.x )  +", "+
				std::to_string( direction.y ) + ", "+
				std::to_string( direction.z ) + "]";

	// >= instead of > to avoid edge case, no rotation would happen with abs direction[0] equal to abs direction[1]
	if ( std::abs( direction[0] ) >= std::abs( direction[1] ) ) { // horizontal movement
		if ( direction[0] > 0 ) {
			leapManager->rotateCamera( 0, 0, 1, 0, 0.01f );
		}
		else if ( direction[0] < 0 ) {
			leapManager->rotateCamera( 0, 0, 1, 0, -0.01f );
		}
	}
	else { // vertical movement
		if ( direction[1] > 0 ) {
			leapManager->rotateCamera( 0, 0,1, 0.01f, 0 );
		}
		else if ( direction[1] < 0 ) {
			leapManager->rotateCamera( 0, 0, 1, -0.01f, 0 );
		}
	}
}

void LeapLib::LeapActions::changeViewAngle( LeapLib::DirectionDetector::Direction direction )
{

	switch ( direction ) {
		case LeapLib::DirectionDetector::Direction::LEFT :
			leapManager->rotateCamera( 0, 0, 1, 0, -0.01f );
			break;
		case LeapLib::DirectionDetector::Direction::RIGHT :
			leapManager->rotateCamera( 0, 0, 1, 0, 0.01f );
			break;
		case LeapLib::DirectionDetector::Direction::UP :
			leapManager->rotateCamera( 0, 0, 1, -0.01f, 0 );
			break;
		case LeapLib::DirectionDetector::Direction::DOWN :
			leapManager->rotateCamera( 0, 0, 1, 0.01f, 0 );
			break;
		case LeapLib::DirectionDetector::Direction::STEADY :
			// stuff
			break;
		default :
			break;
	}

}

void LeapLib::LeapActions::startMovingForward()
{
	leapManager->enableCameraMovement( LeapLib::LeapManager::Movement::FORWARD );
}

void LeapLib::LeapActions::stopMovingForward()
{
	leapManager->disableCameraMovement();
}

//jurik
void LeapLib::LeapActions::graphRotateSwipe( int swipeDirection )
{

	leapManager->graphRotateSwipe( swipeDirection );
}

void LeapLib::LeapActions::rotateAruco( LeapLib::DirectionDetector::Direction direction )
{
	switch ( direction ) {
		case LeapLib::DirectionDetector::Direction::LEFT :
			leapManager->rotateArucoLeft();
			break;
		case LeapLib::DirectionDetector::Direction::RIGHT :
			leapManager->rotateArucoRight();
			break;
		case LeapLib::DirectionDetector::Direction::STEADY :
			break;
		case LeapLib::DirectionDetector::Direction::UP :
			break;
		case LeapLib::DirectionDetector::Direction::DOWN :
			break;
		default :
			break;
	}
}

//void Leap::LeapActions::scaleGraph( Leap::DirectionDetector::Direction direction ){

//    LOG (INFO) << direction;
//    switch (direction) {
//        case Leap::DirectionDetector::Direction::UP :
//            LOG (INFO) << "ScaleNodesUp";
//            leapManager->scaleGraph(true);
//            break;
//        case Leap::DirectionDetector::Direction::DOWN :
//            LOG (INFO) << "ScaleNodesDown";
//            leapManager->scaleGraph(false);
//            break;
//        }
//}

void LeapLib::LeapActions::scaleGraph( Leap::Vector* vec )
{
	double vectorValue =static_cast<double>( vec->x+vec->y+vec->z );
	if ( vectorValue<-150 || vectorValue >150 ) {
		if ( ( vec->x+vec->y+vec->z )>0 ) {
			LOG( INFO ) << "ScaleGraphUp";
			leapManager->scaleGraph( true );
		}
		else {
			LOG( INFO ) << "ScaleGraphDown";
			leapManager->scaleGraph( false );
		}
	}
}

void LeapLib::LeapActions::scaleEdges( LeapLib::DirectionDetector::Direction direction )
{
	switch ( direction ) {
		case LeapLib::DirectionDetector::Direction::LEFT :
			leapManager->scaleEdgesUp();
			break;
		case LeapLib::DirectionDetector::Direction::RIGHT :
			leapManager->scaleEdgesDown();
			break;
		case LeapLib::DirectionDetector::Direction::STEADY :
			break;
		case LeapLib::DirectionDetector::Direction::UP :
			break;
		case LeapLib::DirectionDetector::Direction::DOWN :
			break;
		default :
			break;
	}
}
void LeapLib::LeapActions::scaleNodes( bool scaleUp )
{
	leapManager->scaleNodes( scaleUp );
}

void LeapLib::LeapActions::selectNode( Leap::Hand hand )
{
	leapManager->selectNode( hand.isRight() );

}
