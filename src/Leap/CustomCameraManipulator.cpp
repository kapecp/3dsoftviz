#include "Leap/CustomCameraManipulator.h"

Leap::CustomCameraManipulator::CustomCameraManipulator( Vwr::CameraManipulator* cameraManipulator,Layout::LayoutThread* layout,
		Vwr::CoreGraph* coreGraph )
{
	this->cameraManipulator = cameraManipulator;
	this->layout = layout;
	this->coreGraph = coreGraph;
}

Leap::CustomCameraManipulator::~CustomCameraManipulator( void )
{
	//
}

void Leap::CustomCameraManipulator::enableCameraMovement( Movement direction )
{
	this->cameraManipulator->enableCameraMovement( static_cast<Vwr::CameraManipulator::Movement>( direction ) );
}

void Leap::CustomCameraManipulator::disableCameraMovement()
{
	this->cameraManipulator->disableCameraMovement();
}

void Leap::CustomCameraManipulator::rotateCamera( float py0, float px0, double throwScale, float py1, float px1 )
{
	this->cameraManipulator->rotateCamera( py0,px0,throwScale,py1, px1 );
}

//jurik
void Leap::CustomCameraManipulator::graphRotateSwipe( int swipeDirection )
{
	//direction -1 = LEFT, 1 = RIGHT
	switch ( swipeDirection ) {
		case -1: {
			coreGraph->rotateGraph( swipeDirection );
		}
		case 1: {
			coreGraph->rotateGraph( swipeDirection );
		}
	}
}

void Leap::CustomCameraManipulator::rotateArucoLeft()
{
	coreGraph->rotateGraph( 1 );
}

void Leap::CustomCameraManipulator::rotateArucoRight()
{
	coreGraph->rotateGraph( -1 );
}

void Leap::CustomCameraManipulator::scaleEdgesUp()
{

	float distance = layout->getAlg()->getMaxDistance();

	layout->pause();
	coreGraph->setNodesFreezed( true );
	layout->getAlg()->setMaxDistance( distance * 1.02 );
	coreGraph->scaleGraphToBase();
	coreGraph->setNodesFreezed( false );
	layout->play();
}

void Leap::CustomCameraManipulator::scaleEdgesDown()
{

	float distance = layout->getAlg()->getMaxDistance();

	layout->pause();
	coreGraph->setNodesFreezed( true );
	layout->getAlg()->setMaxDistance( distance * 0.98 );
	coreGraph->scaleGraphToBase();
	coreGraph->setNodesFreezed( false );
	layout->play();
}

void Leap::CustomCameraManipulator::scaleNodes( bool scaleUp )
{
	if ( scaleUp ) {
		coreGraph->scaleNodes( true );
	}
	else {
		coreGraph->scaleNodes( false );
	}
}

//*****
