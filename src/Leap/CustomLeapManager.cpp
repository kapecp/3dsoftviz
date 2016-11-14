#include "Leap/CustomLeapManager.h"

Leap::CustomLeapManager::CustomLeapManager( Vwr::CameraManipulator* cameraManipulator, Layout::LayoutThread* layout,
		Vwr::CoreGraph* coreGraph , osg::ref_ptr<osg::Group> handsGroup )
{
	this->cameraManipulator = cameraManipulator;
	this->layout = layout;
	this->coreGraph = coreGraph;
	this->handsGroup = handsGroup;
	arMode = false;

    //init handPalms here
    // TODO upravit navrh.. manager v sebe nemoze mat ako atributy transformacne matice
    if ( this->handsGroup != NULL ) {
        arMode = true;

        osg::ref_ptr<osg::Geode> leftHandGeode( new osg::Geode );
        osg::ref_ptr<osg::Sphere>  leftHandSphere = new osg::Sphere( osg::Vec3f( 0,0,0 ),0.1f );

        osg::ref_ptr<osg::ShapeDrawable> leftHandDrawable( new osg::ShapeDrawable( leftHandSphere.get() ) );
        leftHandGeode->addDrawable( leftHandDrawable.get() );

        osg::ref_ptr<osg::Geode> rightHandGeode( new osg::Geode );
        osg::ref_ptr<osg::Sphere> rightHandSphere = new osg::Sphere( osg::Vec3f( 0,0,0 ),0.1f );
        osg::ref_ptr<osg::ShapeDrawable> rightHandDrawable( new osg::ShapeDrawable( rightHandSphere.get() ) );
        rightHandGeode->addDrawable( rightHandDrawable.get() );

        osg::ref_ptr<osg::MatrixTransform> transformLeft = new osg::MatrixTransform();
        osg::ref_ptr<osg::MatrixTransform> transformRight = new osg::MatrixTransform();
        transformLeft->addChild( leftHandGeode.get() );
        transformRight->addChild( rightHandGeode.get() );

        transformLeft->setMatrix( osg::Matrix::translate( -0.5,0,-0.5 ) );
        transformRight->setMatrix( osg::Matrix::translate( 0.5,0,-0.5 ) );

        HandPalm *rightPalm = new HandPalm(transformRight);
        HandPalm *leftPalm = new HandPalm(transformLeft);

        handsGroup->insertChild(0, rightPalm->matrixTransform);
        handsGroup->insertChild(1, leftPalm->matrixTransform);
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
        osg::MatrixTransform* rightPalm = dynamic_cast<osg::MatrixTransform*> (handsGroup->getChild(1));
        osg::MatrixTransform* leftPalm = dynamic_cast<osg::MatrixTransform*> (handsGroup->getChild(0));
        rightPalm->setMatrix( osg::Matrix::translate( static_cast<double>( lVector.x )/100.0,static_cast<double>( -lVector.z )/100.0,static_cast<double>( lVector.y )/100.0 ) );
        leftPalm->setMatrix( osg::Matrix::translate( static_cast<double>( rVector.x )/100.0,static_cast<double>( -rVector.z )/100.0,static_cast<double>( rVector.y )/100.0 ) );
	}
}

void initPalms();
