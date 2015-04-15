#include "Kinect/KinectHandTracker.h"
#include "Viewer/MouseControl.h"
#include "Math/CameraMath.h"
#include "QDebug"

#ifdef NITE2_FOUND

Kinect::KinectHandTracker::KinectHandTracker( openni::Device* device, openni::VideoStream* m_depth )
{
	// create hand tracking from device
	m_pHandTracker.create( device );

	// add automatec gesture from Openni to track
	m_pHandTracker.startGestureDetection( nite::GESTURE_WAVE );
	m_pHandTracker.startGestureDetection( nite::GESTURE_CLICK );
	isClick=false;
	isCursorMovementEnable=true;
	mSpeed=1.0;
	mouse = new Vwr::MouseControl();
	mDepth=m_depth;
	viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
    tempSelectedNode = NULL;
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
	// get frame - depth data
	this->m_pHandTracker.readFrame( &this->handTrackerFrame );

	// get automatic gesture - Wave and Click
	const nite::Array<nite::GestureData>& gestures = this->handTrackerFrame.getGestures();
	for ( int i = 0; i < gestures.getSize(); ++i ) {
		// checking for complete gesture
		if ( gestures[i].isComplete() ) {
			printf( "completed gesture\n" );
			if ( gestures[i].getType()==nite::GESTURE_CLICK ) {
				printf( "gesture click is on\n" );
				if ( isClick ) {
					isClick=false;
					printf( "Release\n" );
					mouse->releasePressMouse( Qt::LeftButton );
				}
				else {
					isClick=true;
					printf( "Click\n" );
					mouse->clickPressMouse( Qt::LeftButton );
				}

                // marak start
                if ( tempSelectedNode != NULL ){
                    // restore color settings
                    tempSelectedNode->setDefaultColor();
                    tempSelectedEdge->setEdgeDefaultColor();
                    // add new picked node
                    viewer->getPickHandler()->addPickedNode( tempSelectedNode );
                    // set selected color
                    tempSelectedNode->setSelected( true );
                    // remove tmp node and edge
                    printf("ADDED > %d\n",tempSelectedNode->getId());

                    tempSelectedNode = NULL;
                    tempSelectedEdge = NULL;
                }
			}
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
				mouse->moveCursorWorldCoordinates( mDepthX,mDepthY,isClick );
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

void Kinect::KinectHandTracker::visualSelection( )
{
    QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewer->getPickHandler()->getSelectedNodes();

    // if there is selected node
    if ( selectedNodes->size() != 0 ){
        // get coords of mouse
        float mouseX = static_cast<float>( viewer->cursor().pos().x() - viewer->pos().x());
        float mouseY = static_cast<float>( viewer->height() + viewer->pos().y() - viewer->cursor().pos().y() + 20 );
      //  int mouseZ = 0;

        Data::Node* lastSelectedNode = selectedNodes->last();

       // printf("M > %.3f ; %.3f\n",mouseX, mouseY);

        QMap<qlonglong, osg::ref_ptr<Data::Edge> >* nodeEdges = lastSelectedNode->getEdges();
        Data::Edge* closestNodeEdge = NULL;
        float minDistance = 0;
        printf("L > %d\n",lastSelectedNode->getId());

        for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iter = nodeEdges->begin(); iter != nodeEdges->end(); iter++ ){
            // get dest node and possition
            Data::Node* dstNode = ( *iter)->getOtherNode( lastSelectedNode );
            // convert node coordinates to screen coordinates
            osg::Vec3f nodePossition = camMath->projectOnScreen( viewer->getCamera(), dstNode->getCurrentPosition());
           // printf("NODE %d ",dstNode->getId());

            // calculate distance between node and cursor
            double distance;
            distance = sqrt ( pow( abs( nodePossition[0] - mouseX ), 2)
                            + pow( abs( nodePossition[1] - mouseY ), 2));
                            //+ pow( abs( nodePossition[2] - mouseZ ), 2));

            // first edge or nearer node
            if( ( minDistance == 0) || ( minDistance > distance ) ){
                minDistance = distance;
                closestNodeEdge = ( *iter);
            }

           // printf("> %.3f ; %.3f\n", nodePossition[0], nodePossition[1]);
        }

        // get closest node
        Data::Node* tmpNode = closestNodeEdge->getOtherNode( lastSelectedNode );
        if ( !tmpNode->equals( tempSelectedNode ) ){
            // remove previous temp select
            if( tempSelectedNode != NULL ){
                tempSelectedNode->setDefaultColor();
                tempSelectedEdge->setEdgeDefaultColor();
            }

            // create new temp select for node and edge
            tempSelectedNode = tmpNode;
            tempSelectedNode->setColor( osg::Vec4(1.0f,1.0f,0.0f,0.5f) );
            tempSelectedEdge = closestNodeEdge;
            tempSelectedEdge->setEdgeColor(  osg::Vec4(1.0f,1.0f,0.0f,0.5f) );
        }
    }
    else if( tempSelectedNode != NULL ){
        tempSelectedNode->setDefaultColor();
        tempSelectedNode = NULL;
        tempSelectedEdge->setEdgeDefaultColor();
        tempSelectedEdge = NULL;
    }
}
