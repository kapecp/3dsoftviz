#include "Leap/CustomLeapManager.h"

#include "Viewer/CameraManipulator.h"
#include "Viewer/CoreGraph.h"
#include "Layout/LayoutThread.h"
#include "Layout/FRAlgorithm.h"
#include "HandModule/Model/HandPalm.h"
#include "Network/Server.h"

#include <easylogging++.h>
#include <math.h>
#include <qdatetime.h>

Leap::CustomLeapManager::CustomLeapManager( Vwr::CameraManipulator* cameraManipulator, Layout::LayoutThread* layout,
		Vwr::CoreGraph* coreGraph, osg::ref_ptr<osg::Group> handsGroup )
	:cameraManipulator( cameraManipulator ), layout( layout ), coreGraph( coreGraph ), handsGroup( handsGroup )
{
	arMode = false;
	this->handObjectManipulator = new Leap::HandObjectManipulator( new Leap::HandMapper( this->coreGraph ), 'z' );
	//init handPalms here
	if ( this->handsGroup != nullptr ) {
		arMode = true;
		HandPalm* rightPalm = new HandPalm( 0.1f, handsGroup, HandColors::RIGHT );
		HandPalm* leftPalm = new HandPalm( 0.1f, handsGroup, HandColors::LEFT );

		rightPalm->setMatrix( osg::Matrix::translate( -0.5,0,0 ) );
		leftPalm->setMatrix( osg::Matrix::translate( 0.5,0,0 ) );
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

int Leap::CustomLeapManager::updateCoreGraphBackground( const unsigned char* buffer, float depth )
{
	if ( this->coreGraph->isLeapStreamActive() ) {
		this->coreGraph->updateBackgroundStream( ( unsigned char* )buffer );
	}
	return 1;
}

QDateTime lastSendTime;
void Leap::CustomLeapManager::updateHands( Leap::Hand leftHand, Leap::Hand rightHand )
{
	if ( this->handsGroup != nullptr ) {
		//0 a 3 z dovodu ze v grupe je palmNode, fingerGroup, palmNode, fingerGroup
		HandPalm* leftPalm = static_cast<HandPalm*>( handsGroup->getChild( 3 ) );
		HandPalm* rightPalm = static_cast<HandPalm*>( handsGroup->getChild( 0 ) );

		this->handObjectManipulator->updateHands( leftHand, rightHand, leftPalm, rightPalm, this->coreGraph->getCamera() );

		if ( leftHand.isValid() || rightHand.isValid()) {
			QDateTime dt = QDateTime::currentDateTime();
			if ( lastSendTime.addMSecs( 100 ) <= dt ) {
				lastSendTime = dt;
				Network::Server::getInstance()->invokeSendHands( leftPalm, rightPalm );
			}
		}
	}

}

void Leap::CustomLeapManager::updateHands( QDataStream* stream )
{
	if ( this->handsGroup != nullptr ) {
		//0 a 3 z dovodu ze v grupe je palmNode, fingerGroup, palmNode, fingerGroup
		HandPalm* leftPalm = static_cast<HandPalm*>( handsGroup->getChild( 3 ) );
		HandPalm* rightPalm = static_cast<HandPalm*>( handsGroup->getChild( 0 ) );

		leftPalm->setFromStream( stream );
		rightPalm->setFromStream( stream );

		//this->handObjectManipulator->updateHands( leftHand, rightHand, leftPalm, rightPalm, this->coreGraph->getCamera() );
	}
}

void Leap::CustomLeapManager::scaleGraph( bool scaleUp )
{
	if ( scaleUp ) {
		this->coreGraph->scaleGraph( 2 );
	}
	else {
		this->coreGraph->scaleGraph( 1 );
	}
}

void Leap::CustomLeapManager::selectNode( bool right )
{

//	Treba vyriesit scale ruky, pretoze vzhladom na graf je moc mala a teda je takmer nemozne pretnut ruku s grafom.

//	if ( coreGraph->getHandsGroup() != NULL && coreGraph->getGraph() != NULL) {

//		LOG( INFO ) << "Leap::CustomLeapManager::selectNode";

//		HandPalm* palm = static_cast<HandPalm*>( coreGraph->getHandsGroup()->getChild( right ? 0 : 3 ) );

//		Leap::Joint* tip =  static_cast<Leap::Joint*>( palm->fingerGroup->getChild( static_cast<unsigned int>( 1 ) )->asGroup()->getChild( 0 ));
//		LOG( INFO ) << coreGraph->getGraph()->getNodes()->count();
//		foreach (osg::ref_ptr<Data::Node> node, coreGraph->getGraph()->getNodes()->values()) {

//			LOG( INFO ) << node.get()->getCurrentPosition().x();
//			LOG( INFO ) << tip->getBound().center().x();

//			if ( tip->getBound().intersects(node.get()->getBound())){
//				LOG( INFO ) << "Intersection";
//			}

//		}
//		LOG( INFO ) << tip->getBound().radius();

//	}



}
