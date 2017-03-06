#include "Kinect/KinectHandTracker.h"
#include "Viewer/MouseControl.h"
#include "QDebug"
#include "Viewer/GraphNavigation.h"

#ifdef NITE2_FOUND

Kinect::KinectHandTracker::KinectHandTracker( openni::Device* device, openni::VideoStream* m_depth ) :
	numHandsTracking( 0 ),
	isGestureClick( false ),
	//handZ{0, 0} //C++11
	//getArrayHands{{0, 0}, {0, 0}} //C++11
	slidingHand_x( 0 ),
	slidingHand_y( 0 ),
	slidingHand_z( 0 ),
	slidingHand_type( nullptr ),
	isClick( false ),

	gesto_dolava( false ),
	gesto_doprava( false ),
	gesto_hore( false ),
	gesto_dole( false ),
	viewer( AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt() ),
	nav( nullptr ),
	isCursorMovementEnable( true ),
	mouse( new Vwr::MouseControl() ),
	mDepth( m_depth ),
	mDepthX( 0 ),
	mDepthY( 0 ),
	mDepthZ( 0 ),
	mSpeed( 1.0 )
{
	// create hand tracking from device
	m_pHandTracker.create( device );

	// add automatec gesture from Openni to track
	m_pHandTracker.startGestureDetection( nite::GESTURE_WAVE );
	m_pHandTracker.startGestureDetection( nite::GESTURE_CLICK );
	handZ[0] = 0.0f;
	handZ[1] = 0.0f;

	getArrayHands[0][0] = 0.0f;
	getArrayHands[0][1] = 0.0f;
	getArrayHands[1][0] = 0.0f;
	getArrayHands[1][1] = 0.0f;
}


Kinect::KinectHandTracker::~KinectHandTracker()
{
}

void Kinect::KinectHandTracker::setCursorMovement( bool set )
{
	isCursorMovementEnable=set;
}

void Kinect::KinectHandTracker::setSpeedMovement( double set )
{
	mSpeed=set;
}

void Kinect::KinectHandTracker::getAllGestures()
{
	isGestureClick = false;
	// get frame - depth data
	this->m_pHandTracker.readFrame( &this->handTrackerFrame );

	// get automatic gesture - Wave and Click
	const nite::Array<nite::GestureData>& gestures = this->handTrackerFrame.getGestures();
	for ( int i = 0; i < gestures.getSize(); ++i ) {
		// checking for complete gesture
		if ( gestures[i].isComplete() ) {
			printf( "completed gesture\n" );
			/*
			 * if ( gestures[i].getType()==nite::GESTURE_CLICK ) {
				printf( "gesture click is on\n" );
				if ( isClick ) {
					isClick=false;
					printf( "Release\n" );
				}
				else {
					isClick=true;
					printf( "Click\n" );
				}
				isGestureClick = true;

			}*/
			const nite::Point3f& position = gestures[i].getCurrentPosition();
			printf( "Gesture %d at (%f,%f,%f)\n", gestures[i].getType(), position.x, position.y, position.z );

			// for better tracking
			nite::HandId newId;
			this->m_pHandTracker.startHandTracking( gestures[i].getCurrentPosition(), &newId );
		}
	}
}

void Kinect::KinectHandTracker::getAllHands()
{
	// List of hands evidence
	// If hand matches old positions, previous ID is assigned again
	const nite::Array<nite::HandData>& hands= this->handTrackerFrame.getHands();

	//  printf("%d hands\n", hands.getSize());

	for ( int i = 0; i < hands.getSize(); ++i ) {
		const nite::HandData& user = hands[i];

		if ( !user.isTracking() ) {
			printf( "Lost hand %d\n", user.getId() );
			nite::HandId id = user.getId();
			HistoryBuffer<20>* pHistory = this->g_histories[id];
			this->g_histories.erase( this->g_histories.find( id ) );
			delete pHistory;
		}
		else {
			if ( user.isNew() ) {
				printf( "New hand id %d\n", user.getId() );
				this->g_histories[user.getId()] = new HistoryBuffer<20>;
			}
			// Hand evidence in Buffer
			HistoryBuffer<20>* pHistory = this->g_histories[user.getId()];
			pHistory->AddPoint( user.getPosition() );

			//first find HAND = MOUSE
			if ( i==0 && isCursorMovementEnable ) {
				mouse->setSpeedUpMoving( mSpeed );
				coordinateConverter.convertWorldToDepth( *mDepth, user.getPosition().x, user.getPosition().y, user.getPosition().z, &mDepthX, &mDepthY, &mDepthZ );
				mouse->moveCursorWorldCoordinates( mDepthX,mDepthY, true );
			}

			// TODO - further implementation should include depth information in pixels
			this->getArrayHands[i][0] = user.getPosition().x;
			this->getArrayHands[i][1] = 0 - user.getPosition().y;
			this->handZ[i] = user.getPosition().z;
		}
	}
	// Set number of hands in tracking
	this->numHandsTracking = hands.getSize();
	//printf( "%d hands\n", hands.getSize() );
}

void Kinect::KinectHandTracker::getRotatingMove()
{
	// List of hands evidence
	// If hand matches old positions, previous ID is assigned again
	const nite::Array<nite::HandData>& hands= this->handTrackerFrame.getHands();

	this->slidingHand_x = 0;
	this->slidingHand_y = 0;
	this->slidingHand_z = 0;
	this->slidingHand_type = "";
	//strcpy(this->slidingHand_type, "");

//	for ( int i = 0; i < hands.getSize(); ++i ) {
	if ( hands.getSize() != 0 ) {
		//const nite::HandData& user = hands[i];
		const nite::HandData& user = hands[0];
		if ( !user.isTracking() ) {
			//printf("Lost hand %d\n", user.getId());
			nite::HandId id = user.getId();
			HistoryBuffer<20>* pHistory = this->g_histories[id];
			this->g_histories.erase( this->g_histories.find( id ) );
			delete pHistory;
		}
		else {
			if ( user.isNew() ) {
				this->g_histories[user.getId()] = new HistoryBuffer<20>;
			}
			// Hand evidence in Buffer
			HistoryBuffer<20>* pHistory = this->g_histories[user.getId()];
			pHistory->AddPoint( user.getPosition() );

			coordinateConverter.convertWorldToDepth( *mDepth, user.getPosition().x, user.getPosition().y, user.getPosition().z, &this->slidingHand_x, &this->slidingHand_y, &this->slidingHand_z );

			float koordy[60] = {0};

			if ( pHistory->GetSize() == 20 ) {
				// ak je historia naplnena
				const nite::Point3f& position1 = pHistory->operator[]( 0 );
				this->m_pHandTracker.convertHandCoordinatesToDepth( position1.x, position1.y, position1.z, &koordy[0], &koordy[1] );
				const nite::Point3f& position2 = pHistory->operator[]( 19 );
				this->m_pHandTracker.convertHandCoordinatesToDepth( position2.x, position2.y, position2.z, &koordy[2], &koordy[3] );

				gesto_dolava = false;
				gesto_doprava = false;
				gesto_hore = false;
				gesto_dole = false;

				//check for output
				if ( abs( koordy[0] - koordy[2] ) > 100.0 ) {
					if ( koordy[0] > koordy[2] ) {
						gesto_doprava = true;
					}
					else {
						gesto_dolava = true;
					}
				}
				if ( abs( koordy[1] - koordy[3] ) > 100.0 ) {
					if ( koordy[1] < koordy[3] ) {
						gesto_hore = true;
					}
					else {
						gesto_dole = true;
					}
				}
				if ( gesto_dolava ) {
					this->slidingHand_type = "scroll left";
				}
				else if ( gesto_doprava ) {
					this->slidingHand_type = "scroll right";
				}
				else if ( gesto_hore ) {
					this->slidingHand_type = "scroll up";
				}
				else if ( gesto_dole ) {
					this->slidingHand_type = "scroll down";
				}
			}
		}
	}
}

// set event for graph movement
void Kinect::KinectHandTracker::moveGraphByHand( )
{
	if ( gesto_dolava ) {
		viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Left );
		viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Left );
	}
	else if ( gesto_doprava ) {
		viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Right );
		viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Right );
	}
	if ( gesto_hore ) {
		viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Up );
		viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Up );
	}
	else if ( gesto_dole ) {
		viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Down );
		viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Down );
	}
}
void Kinect::KinectHandTracker::moveGraphByHandToDepth( float deltaZ )
{
	// trashold
	if ( abs( deltaZ ) > 7.0f ) {
		if ( deltaZ > 0 ) {
			viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Up );
			viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Up );
		}
		else {
			viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Down );
			viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Down );
		}
	}
}
#endif // NITE2_FOUND
