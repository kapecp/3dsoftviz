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

    if ( this->handsGroup != NULL ) {
        HandPalm* rightPalm = dynamic_cast<HandPalm*>(handsGroup->getChild(0));
        HandPalm* leftPalm = dynamic_cast<HandPalm*>(handsGroup->getChild(1));

        leftPalm->setMatrix(
                    osg::Matrix::translate( static_cast<double>( lVector.x )/100.0,static_cast<double>( -lVector.z )/100.0,
                                            static_cast<double>( lVector.y )/100.0 ));
        rightPalm->setMatrix(
                    osg::Matrix::translate( static_cast<double>( rVector.x )/100.0,static_cast<double>( -rVector.z )/100.0,
                                            static_cast<double>( rVector.y )/100.0 ) );

        this->updateFingers(rightPalm, rightHand.fingers());
        this->updateFingers(leftPalm, leftHand.fingers());
    }
}

void Leap::CustomLeapManager::updateFingers(HandPalm* palm, Leap::FingerList fingers) {

//    int i = 0;
//    std::list<Joint*>::iterator it = palm->coreJoints.begin();
//    for(i = 0; i < fingers.count(); i++)
//    {
//       updateFinger((*it), fingers[i]);
//       std::next(it, 1);
//    }

    int i = 0;
    for(i = 0; i < fingers.count(); i++) {
        updateFinger(static_cast<Joint*>(palm->getChild(i)), fingers[i]);
    }

}

void Leap::CustomLeapManager::updateFinger(Joint* joint, Leap::Finger finger) {
    //TODO zaciatky prstov su uz v scene ale len na pozici 0,0,0(palmGroup) - treba im nastavit poziciu

//    finger.bone(1)->prevJoint();
    LOG (INFO) << "Leap/CustomLeapManager/updateFinger() " + std::to_string(finger.type());

}
