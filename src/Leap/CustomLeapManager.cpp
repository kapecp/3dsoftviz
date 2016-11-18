#include "Leap/CustomLeapManager.h"
#include <easylogging++.h>

Leap::CustomLeapManager::CustomLeapManager( Vwr::CameraManipulator* cameraManipulator, Layout::LayoutThread* layout,
		Vwr::CoreGraph* coreGraph , osg::ref_ptr<osg::Group> handsGroup )
{
	this->cameraManipulator = cameraManipulator;
	this->layout = layout;
	this->coreGraph = coreGraph;
	this->handsGroup = handsGroup;
	arMode = false;

    //init handPalms here
    if ( this->handsGroup != NULL ) {
        arMode = true;
        HandPalm *rightPalm = new HandPalm(0.1f, handsGroup);
        HandPalm *leftPalm = new HandPalm(0.1f, handsGroup);

        rightPalm->setMatrix(osg::Matrix::translate( -0.5,0,-0.5 ));
        leftPalm->setMatrix(osg::Matrix::translate( 0.5,0,-0.5 ));
    }

}

Leap::CustomLeapManager::~CustomLeapManager( void )
{
	//remove hands nodes here
	if ( handsGroup ) {
		handsGroup->removeChildren( 0,handsGroup->getNumChildren() );
	}
}

void Leap::CustomLeapManager::enableCameraMovement( Movement direction )
{
	this->cameraManipulator->enableCameraMovement( static_cast<Vwr::CameraManipulator::Movement>( direction ) );
}

void Leap::CustomLeapManager::disableCameraMovement()
{
	this->cameraManipulator->disableCameraMovement();
}

void Leap::CustomLeapManager::rotateCamera( float py0, float px0, double throwScale, float py1, float px1 )
{
	this->cameraManipulator->rotateCamera( py0,px0,throwScale,py1, px1 );
}

//jurik
void Leap::CustomLeapManager::graphRotateSwipe( int swipeDirection )
{
	//direction -1 = LEFT, 1 = RIGHT
	switch ( swipeDirection ) {
		case -1: {
			coreGraph->rotateGraph( swipeDirection );
			break;
		}
		case 1: {
			coreGraph->rotateGraph( swipeDirection );
			break;
		}
		default:
			break;
	}
}

void Leap::CustomLeapManager::rotateArucoLeft()
{
	coreGraph->rotateGraph( 1 );
}

void Leap::CustomLeapManager::rotateArucoRight()
{
	coreGraph->rotateGraph( -1 );
}

#include <easylogging++.h>

void Leap::CustomLeapManager::scaleEdgesUp()
{

	float distance = layout->getAlg()->getMaxDistance();

	layout->pause();
	coreGraph->setNodesFreezed( true );
	layout->getAlg()->setMaxDistance( distance * 1.02f );
	coreGraph->scaleGraphToBase();
	coreGraph->setNodesFreezed( false );
	layout->play();
}

void Leap::CustomLeapManager::scaleEdgesDown()
{

	float distance = layout->getAlg()->getMaxDistance();

	layout->pause();
	coreGraph->setNodesFreezed( true );
	layout->getAlg()->setMaxDistance( distance * 0.98f );
	coreGraph->scaleGraphToBase();
	coreGraph->setNodesFreezed( false );
	layout->play();
}

void Leap::CustomLeapManager::scaleNodes( bool scaleUp )
{
	if ( scaleUp ) {
		coreGraph->scaleNodes( true );
	}
	else {
		coreGraph->scaleNodes( false );
	}
}

//*****
void Leap::CustomLeapManager::updateHands( Leap::Hand leftHand, Leap::Hand rightHand )
{        
    Leap::Vector lVector = Leap::Vector(0.0f,0.0f,0.0f);
    if ( leftHand.isValid() ) {
        lVector = leftHand.palmPosition();
    }
    Leap::Vector rVector = Leap::Vector(0.0f,0.0f,0.0f);
    if ( rightHand.isValid() ) {
        rVector = rightHand.palmPosition();
    }
//    LOG (INFO) << lVector.toString() << ' ';

    if ( this->handsGroup != NULL ) {
        //0 a 2 z dovodu ze v grupe je palmNode, fingerGroup, palmNode, fingerGroup
        HandPalm* rightPalm = static_cast<HandPalm*>(handsGroup->getChild(0));
        HandPalm* leftPalm = static_cast<HandPalm*>(handsGroup->getChild(2));

        leftPalm->setMatrix(
                    osg::Matrix::translate( static_cast<double>( lVector.x )/100.0,
                                            static_cast<double>( -lVector.z )/100.0,
                                            static_cast<double>( lVector.y )/100.0 ));
        rightPalm->setMatrix(
                    osg::Matrix::translate( static_cast<double>( rVector.x )/100.0,
                                            static_cast<double>( -rVector.z )/100.0,
                                            static_cast<double>( rVector.y )/100.0 ) );

        this->updateFingers(rightPalm, rightHand.fingers());
        this->updateFingers(leftPalm, leftHand.fingers());
    }
}

void Leap::CustomLeapManager::updateFingers(HandPalm* palm, Leap::FingerList fingers) {
    int i = 0;
    // odstranene fingers.count(), aby sa pozicie klbov zobrazili na 0,0,0, ked nieje zachytavana ruka
    for(i = 0; i < 5; i++) {
        updateFinger(static_cast<osg::Group*>(palm->fingerGroup->getChild(i)->asGroup()), fingers[i]);
    }

}

void Leap::CustomLeapManager::updateFinger(osg::Group*  fingerGroup, Leap::Finger fingerLeap) {
    // vykreslenie klbov zapastia
    //
    Joint* joint = static_cast<Joint*>(fingerGroup->getChild(0));
    Leap::Vector posVector = Leap::Vector(0.0f,0.0f,0.0f);
    if(fingerLeap.bone(static_cast<Leap::Bone::Type>(0)).isValid()){
       posVector = fingerLeap.bone(static_cast<Leap::Bone::Type>(0)).prevJoint();
    }
    joint->setMatrix(osg::Matrix::translate( static_cast<double>( posVector.x )/100.0,
                                             static_cast<double>( -posVector.z )/100.0,
                                             static_cast<double>( posVector.y )/100.0 ));
    // vykreslenie prstov
    int i = 0;
    for(i = 0; i < 4; i++) {
        Joint* joint = static_cast<Joint*>(fingerGroup->getChild(i+1));

        Leap::Vector posVector = Leap::Vector(0.0f,0.0f,0.0f);
        if(fingerLeap.bone(static_cast<Leap::Bone::Type>(i)).isValid()){
           posVector = fingerLeap.bone(static_cast<Leap::Bone::Type>(i)).nextJoint();
        }
        joint->setMatrix(osg::Matrix::translate( static_cast<double>( posVector.x )/100.0,
                                                 static_cast<double>( -posVector.z )/100.0,
                                             static_cast<double>( posVector.y )/100.0 ));
    }

}
