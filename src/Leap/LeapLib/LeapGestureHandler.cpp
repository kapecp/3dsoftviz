#include <easylogging++.h>

#include "LeapLib/LeapGestureHandler.h"
#include "LeapLib/LeapActions.h"
#include "LeapLib/DirectionDetector.h"
#include "LeapLib/FingerPositionDetector.h"

#include <QtGlobal>


LeapLib::LeapGestureHandler::LeapGestureHandler( LeapManager* leapManager )
{
	leapActions = new LeapLib::LeapActions( leapManager );
	this->leapManager = leapManager;
	LOG( INFO ) << "LeapGestureHandler() Constructor";
	this->isStartFrameSet = false;
	this->frameCounter = 0 ;
	startFrame = new Leap::Frame();

}

LeapLib::LeapGestureHandler::~LeapGestureHandler()
{
	if ( leapManager != NULL ) {
		delete ( leapManager );
	}
	LOG( INFO ) << "~LeapGestureHandler() Destructor";
}


void LeapLib::LeapGestureHandler::setStartFrame( Leap::Frame frame )
{
	if ( !this->isStartFrameSet ) {
		*startFrame = frame;
		this->isStartFrameSet = true ;
		LOG( INFO ) << "Leap firstFrame setted.";
	}
}

void LeapLib::LeapGestureHandler::handleGestures( Leap::Frame frame )
{

	Leap::HandList hands = frame.hands();
	LeapLib::DirectionDetector::Direction leftHandDirection ;
	LeapLib::DirectionDetector::Direction rightHandDirection ;
	Leap::Vector* leftHandVelocity = new Leap::Vector( 0,0,0 );
	Leap::Vector* rightHandVelocity = new Leap::Vector( 0,0,0 );
	Leap::Vector* deltaVelocity = new Leap::Vector( 0,0,0 ) ;
	bool leftHandExtended = false;
	bool rightHandExtended = false;
	bool ignoreGestures = false;

	Leap::Hand leftHand, rightHand ;




	for ( int i=0; i< hands.count(); ++i ) {
		if ( hands[i].isRight() ) {
			rightHandDirection = LeapLib::DirectionDetector::getPalmDirection( hands[i] );
			*rightHandVelocity = hands[i].palmVelocity();
			rightHand = hands[i];

			rightHandExtended = LeapLib::FingerPositionDetector::isHandExtended( hands[i] );
		}
		else {
			leftHandDirection = LeapLib::DirectionDetector::getPalmDirection( hands[i] );
			*leftHandVelocity = hands[i].palmVelocity();
			//leapActions->scaleEdges( direction );
			leftHandExtended = LeapLib::FingerPositionDetector::isHandExtended( hands[i] );
			leftHand = hands[i];
		}
	}
	// check if both hands are active
//    if(rightHand.isValid() && !rightHandExtended){
//            this->setStartFrame(frame);
//            LOG(INFO)<<rightHandDirection;
//            //UP,DOWN,LEFT,RIGHT,STEADY
//            leapActions->scaleGraph(rightHandDirection);

//            Leap::Vector leftHandMovement = leftHand.translation(*startFrame);
//            Leap::Vector rightHandMovement = rightHand.translation(*startFrame);
//            float xDiff = leftHandMovement.x - rightHandMovement.x ;
//            float yDiff = leftHandMovement.y - rightHandMovement.y ;
//            float zDiff = leftHandMovement.z - rightHandMovement.z ;
//            //LOG(INFO) << "xdiff:",std::to_string(xDiff) , " yDiff:" ,std::to_string(yDiff), " zDiff:" , std::to_string(zDiff) ;
//            printf("%g  %g  %g\n", leftHandMovement.x, leftHandMovement.y, leftHandMovement.z);
	//Concurrency::wait(250);
//    }
	if ( !qFuzzyCompare( leftHandVelocity->x, 0 ) && !qFuzzyCompare( rightHandVelocity->x, 0 ) ) {
		if ( !leftHandExtended && !rightHandExtended ) {
			ignoreGestures = true;
			deltaVelocity = new Leap::Vector( rightHandVelocity->x - leftHandVelocity->x,rightHandVelocity->y - leftHandVelocity->y,rightHandVelocity->z - leftHandVelocity->z );
			leapActions->scaleGraph( deltaVelocity );
//			Concurrency::wait(50);

		}
	}

	if ( !ignoreGestures ) {

		for ( int i = 0; i < hands.count(); i++ ) {

			if ( LeapLib::FingerPositionDetector::isIndexFingerExtended( hands[i] ) ) {


				leapActions->selectNode( hands[i] );

			}

		}


		const Leap::GestureList gestures = frame.gestures();
		for ( int g = 0; g < gestures.count(); ++g ) {
			Leap::Gesture gesture = gestures[g];
			Leap::HandList hands = gesture.hands();
			Leap::Hand firstHand = hands[0];

			switch ( gesture.type() ) {
				case Leap::Gesture::TYPE_CIRCLE: {
//                LOG( INFO ) << "GESTO CIRCLE....zoomGraph().";
//                leapActions->zoomGraph(gesture);
					break;
				}
				case Leap::Gesture::TYPE_SWIPE: {
					//if (!Leap::FingerPositionDetector::isHandExtended(gesture.hands()[0])){
					LOG( INFO ) << "GESTO swipe ....rotateGraph().";
					leapActions->rotateGraph( gesture );
					//}
					break;

				}
				case Leap::Gesture::TYPE_KEY_TAP: {
					LOG( INFO ) << "GESTO KEY_TAP....onKeyTap().";
//                leapActions->onKeyTap(gesture);
					break;
				}
				case Leap::Gesture::TYPE_SCREEN_TAP: {
					LOG( INFO ) << "GESTO SCREEN_TAP....onScreenTap().";
//                leapActions->onScreenTap(gesture);
					break;
				}
				case Leap::Gesture::TYPE_INVALID: {
					LOG( INFO ) << "GESTO INVALID....onScreenTap().";
//                leapActions->onScreenTap(gesture);
					break;
				}
				default:
					LOG( INFO ) << "Unknown gesture type.";
					break;
			}
		}
	}
}
