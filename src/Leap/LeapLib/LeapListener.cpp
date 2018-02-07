#include "LeapLib//LeapListener.h"

#include "LeapLib/DirectionDetector.h"
#include "LeapLib/FingerPositionDetector.h"

#include "osg/Image"
#include "Leap/CustomLeapManager.h"


//#include "osgDB/WriteFile"
#include <stdio.h>

LeapLib::LeapListener::LeapListener( LeapManager* leapManager )
{
	this->leapGestureHandler = new LeapLib::LeapGestureHandler( leapManager );
	this->leapManager = dynamic_cast<Softviz::Leap::CustomLeapManager*>( leapManager );
	this->arMode = leapManager->arMode;
	LOG( INFO ) << "Leap/LeapLib/LeapListener Constructor";
}

LeapLib::LeapListener::~LeapListener( void )
{

	if ( leapGestureHandler != NULL ) {
		delete ( leapGestureHandler );
	}
}

void LeapLib::LeapListener::onInit( const Leap::Controller& controller )
{

}

void LeapLib::LeapListener::onConnect( const Leap::Controller& controller )
{
	// we put our gestures here to initialize them
	controller.enableGesture( Leap::Gesture::TYPE_CIRCLE );
	controller.enableGesture( Leap::Gesture::TYPE_KEY_TAP );
	controller.enableGesture( Leap::Gesture::TYPE_SCREEN_TAP );
	controller.enableGesture( Leap::Gesture::TYPE_SWIPE );

	controller.config().setFloat( "Gesture.Swipe.MinLength",60.0f );
	controller.config().setFloat( "Gesture.Circle.MinArc", 6.28f );
	controller.config().save();
}

void LeapLib::LeapListener::onDisconnect( const Leap::Controller& controller )
{

}

void LeapLib::LeapListener::onExit( const Leap::Controller& controller )
{

}
void LeapLib::LeapListener::onImages( const Leap::Controller& controller )
{
	Leap::ImageList images = controller.images();
	Leap::Image image = images[0];

	if ( image.data() == NULL ) {
		return;
	}

	Leap::Frame frame = controller.frame();
	Leap::HandList hands = frame.hands();

	this->leapManager->updateCoreGraphBackground( image.data(), 0 );

}

void LeapLib::LeapListener::onFrame( const Leap::Controller& controller )
{
	Leap::Frame frame = controller.frame();
	Leap::HandList hands = frame.hands();
	bool leftHandExtended = false;
	bool rightHandExtended = false;
	Leap::Hand leftHand;
	Leap::Hand rightHand;

	//jurik
	//takin just first gesture (gestures are defined for each finger)
	Leap::Gesture gesture = frame.gestures()[0];

	if ( arMode ) {
		for ( int i=0; i< hands.count(); ++i ) {
			if ( hands[i].isRight() ) {
				rightHand = hands[i];
			}
			else {
				leftHand = hands[i];
			}
		}
		leapManager->updateHands( leftHand, rightHand );
		//leapActions->updateARHands( leftHand,rightHand );
	}
	else {
//<<<<<<< HEAD
//		for ( int i=0; i< hands.count(); ++i ) {
//			if ( hands[i].isRight() ) {
//				direction = Leap::DirectionDetector::getPalmDirection( hands[i] );
//				//using cameramanipulator
//				//leapActions->changeViewAngle( direction );
//				//using pickhandler class
//				leapActions->rotateAruco( direction );

//				if ( gesture.type() == Gesture::TYPE_KEY_TAP ) {
//					leapActions->scaleNodes( true );
//				}
//			}
//			else {
//				direction = Leap::DirectionDetector::getPalmDirection( hands[i] );
//				//leapActions.changeViewAngle( direction );
//				leapActions->scaleEdges( direction );
//				if ( gesture.type() == Gesture::TYPE_KEY_TAP ) {
//					leapActions->scaleNodes( false );
//				}

//				/*handExtended = Leap::FingerPositionDetector::isHandExtended( hands[i] );
//				if ( handExtended ) {
//					leapActions->startMovingForward();
//				}
//				else {
//					leapActions->stopMovingForward();
//				}*/
//			}
//		}
//	}

//	//std::cout << "id: " << frame.id();
//	/*
//		const GestureList gestures = frame.gestures();
//		  for (int g = 0; g < gestures.count(); ++g) {
//			Gesture gesture = gestures[g];

//			HandList hands = gesture.hands();
//			Hand firstHand = hands[0];

//			switch (gesture.type()) {
//			  case Gesture::TYPE_CIRCLE:
//			  {
//				leapActions->zoomGraph(gesture);
//				break;
//			  }
//			  case Gesture::TYPE_SWIPE:
//			  {
//				if(firstHand.isRight()){
//					if(leapActions->isCameraMoving)
//						leapActions->moveCamera(gesture);
//					else
//					  leapActions->rotateGraph(gesture);
//				}
//				break;
//			  }
//			  case Gesture::TYPE_KEY_TAP:
//			  {
//				if(firstHand.isLeft())
//					leapActions->onKeyTap(gesture);
//				break;
//			  }
//			  case Gesture::TYPE_SCREEN_TAP:
//			  {
//				leapActions->onScreenTap(gesture);
//				break;
//			  }
//			  default:
//				qDebug() << "Unknown gesture type.";
//				break;
//			}
//		  }*/


		leapGestureHandler->handleGestures( frame );
	}


}

void LeapLib::LeapListener::onFocusGained( const Leap::Controller& controller )
{
	LOG( INFO ) << "Focus gained.";
}

void LeapLib::LeapListener::onFocusLost( const Leap::Controller& controller )
{
	LOG( INFO ) << "Focus lost.";
}

void LeapLib::LeapListener::onDeviceChange( const Leap::Controller& controller )
{
	const Leap::DeviceList devices = controller.devices();

	for ( int i = 0; i < devices.count(); ++i ) {
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << ( devices[i].isStreaming() ? "true" : "false" ) << std::endl;
	}

}

void LeapLib::LeapListener::onServiceConnect( const Leap::Controller& controller )
{
	LOG( INFO ) << "Service connected.";
}

void LeapLib::LeapListener::onServiceDisconnect( const Leap::Controller& controller )
{
	LOG( INFO ) << "Service disconnect.";
}
