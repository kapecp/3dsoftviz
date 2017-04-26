#include "LeapLib//LeapListener.h"
#include "LeapLib/DirectionDetector.h"
#include "LeapLib/FingerPositionDetector.h"
#include "osg/Image"
#include "CustomLeapManager.h"

//#include "osg/Image"
#include "osgDB/WriteFile"
#include <stdio.h>

Leap::LeapListener::LeapListener( LeapManager* leapManager )
{
    this->leapGestureHandler = new Leap::LeapGestureHandler( leapManager );
    this->leapManager = dynamic_cast<Leap::CustomLeapManager*>( leapManager );
	this->arMode = leapManager->arMode;
	LOG( INFO ) << "Leap/LeapLib/LeapListener Constructor";
}

Leap::LeapListener::~LeapListener( void )
{
    if ( leapGestureHandler != NULL ) {
        delete( leapGestureHandler );
	}
}

void Leap::LeapListener::onInit( const Controller& controller )
{

}

void Leap::LeapListener::onConnect( const Controller& controller )
{
	// we put our gestures here to initialize them
	controller.enableGesture( Gesture::TYPE_CIRCLE );
	controller.enableGesture( Gesture::TYPE_KEY_TAP );
    controller.enableGesture( Gesture::TYPE_SCREEN_TAP );
	controller.enableGesture( Gesture::TYPE_SWIPE );

	controller.config().setFloat( "Gesture.Swipe.MinLength",60.0f );
	controller.config().setFloat( "Gesture.Circle.MinArc", 6.28f );
	controller.config().save();
}

void Leap::LeapListener::onDisconnect( const Controller& controller )
{

}

void Leap::LeapListener::onExit( const Controller& controller )
{

}
void Leap::LeapListener::onImages( const Controller& controller )
{
	ImageList images = controller.images();
	Image image = images[0];

	if ( image.data() == NULL ) {
		return;
	}

    Frame frame = controller.frame();
    HandList hands = frame.hands();
    float depth = this->leapManager->getHandsDepthInformation(hands[0].palmPosition().y, hands[1].palmPosition().y);

    this->leapManager->updateCoreGraphBackground( image.data(), depth);

}

void Leap::LeapListener::onFrame( const Controller& controller )
{
	Frame frame = controller.frame();
    HandList hands = frame.hands();
    bool leftHandExtended = false;
    bool rightHandExtended = false;
	Hand leftHand;
	Hand rightHand;

	//jurik
	//takin just first gesture (gestures are defined for each finger)
	Gesture gesture = frame.gestures()[0];

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
        leapGestureHandler->handleGestures(frame);
    }

}

void Leap::LeapListener::onFocusGained( const Controller& controller )
{
     LOG( INFO ) << "Focus gained.";
}

void Leap::LeapListener::onFocusLost( const Controller& controller )
{
     LOG( INFO ) << "Focus lost.";
}

void Leap::LeapListener::onDeviceChange( const Controller& controller )
{
	const DeviceList devices = controller.devices();

	for ( int i = 0; i < devices.count(); ++i ) {
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << ( devices[i].isStreaming() ? "true" : "false" ) << std::endl;
	}

}

void Leap::LeapListener::onServiceConnect( const Controller& controller )
{
     LOG( INFO ) << "Service connected.";
}

void Leap::LeapListener::onServiceDisconnect( const Controller& controller )
{
     LOG( INFO ) << "Service disconnect.";
}
