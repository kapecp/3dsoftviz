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
    this->leapGestureHandler = new LeapGestureHandler(leapManager); // TODO dava divny error po pridani tohto
    this->leapManager = leapManager;
	this->arMode = leapManager->arMode;
	LOG( INFO ) << "Leap/LeapLib/LeapListener Constructor";
}

Leap::LeapListener::~LeapListener( void )
{
    if ( leapManager != NULL ) {
        delete( leapManager );
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
    dynamic_cast<Leap::CustomLeapManager*>(leapManager)->updateCoreGraphBackground( image.data() ); // toto eventualne pretypovat v konstruktore
}

void Leap::LeapListener::onFrame( const Controller& controller )
{
	Frame frame = controller.frame();
	HandList hands = frame.hands();
	Leap::DirectionDetector::Direction direction;
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
	}
	else {
		for ( int i=0; i< hands.count(); ++i ) {
			if ( hands[i].isRight() ) {
				direction = Leap::DirectionDetector::getPalmDirection( hands[i] );
                //leapActions->rotateAruco( direction );
                rightHandExtended = Leap::FingerPositionDetector::isHandExtended( hands[i] );
			}
			else {
				direction = Leap::DirectionDetector::getPalmDirection( hands[i] );

                //leapActions->scaleEdges( direction );
                leftHandExtended = Leap::FingerPositionDetector::isHandExtended( hands[i] );

			}

		}
	}

//    const GestureList gestures = frame.gestures();
//    for (int g = 0; g < gestures.count(); ++g) {
//        Gesture gesture = gestures[g];

//        HandList hands = gesture.hands();
//        Hand firstHand = hands[0];

//        switch (gesture.type()) {
//        case Gesture::TYPE_CIRCLE:
//        {
//            LOG( INFO ) << "GESTO CIRCLE....zoomGraph().";
//            leapActions->zoomGraph(gesture);
//            break;
//        }
//        case Gesture::TYPE_SWIPE:
//        {

//            LOG( INFO ) << "GESTO swipe ....rotateGraph().";
//            leapActions->rotateGraph(gesture);
//            break;
//        }
//        case Gesture::TYPE_KEY_TAP:
//        {
//            LOG( INFO ) << "GESTO KEY_TAP....onKeyTap().";
//            leapActions->onKeyTap(gesture);
//            break;
//        }
//        case Gesture::TYPE_SCREEN_TAP:
//        {
//            LOG( INFO ) << "GESTO SCREEN_TAP....onScreenTap().";
//            leapActions->onScreenTap(gesture);
//            break;
//        }
//        default:
//            LOG( INFO ) << "Unknown gesture type.";
//            break;
//        }
//    }
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
