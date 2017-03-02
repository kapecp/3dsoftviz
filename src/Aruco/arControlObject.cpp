#include "Aruco/arControlObject.h"
#include "Util/ApplicationConfig.h"
#include <QDebug>
#include <QMap>
#include <QMatrix>

#include "Manager/Manager.h"
#include "Data/Graph.h"
#include "Data/Node.h"
#include "Core/Core.h"
#include "Viewer/CoreGraph.h"



namespace ArucoModul {


ArControlObject::ArControlObject( int id, osg::Vec3f position )
{
	this->id = id;
	this->position = position;
	this->focused = false;
	this->lost = false;

	//Create lost timer
	this->timer = new QTimer( 0 );
	this->timer->setInterval( 3000 );
	this->timer->setSingleShot( true );

	//Move to own thread ... needed by lost timer
	QThread* m_workerThread = new QThread();
	this->moveToThread( m_workerThread );
	this->timer->moveToThread( m_workerThread );
	connect( this->timer, SIGNAL( timeout() ), this, SLOT( timerEvent() ) );
	m_workerThread->start();

	updatePosition( this->position );
}

void ArControlObject::timerEvent()
{
	qDebug() << "LOST MARKER TRACK" ;
	this->lost = true;
	this->focusedNode->setDefaultColor();
	this->focusedNode->setUsingInterpolation( true );
	this->focusedNode->setIgnoreByLayout( false );

	//SELECTION MODE - ONLY PICKED NODE
	/*
	for(auto j : this->focusedNode->getEdges()->keys()){
	    this->focusedNode->getEdges()->value(j)->getOtherNode( this->focusedNode )->setIgnoreByLayout( false );
	    this->focusedNode->getEdges()->value(j)->getOtherNode( this->focusedNode )->setDefaultColor();
	}
	*/
}

void ArControlObject::updatePosition( osg::Vec3f position )
{
	this->position = position;

	if ( !this->focused ) {
		Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

		for ( auto e : allNodes->keys() ) {
			// not already used and marker near node, pick it
			if ( !allNodes->value( e )->isIgnoredByLayout() && chckIfNearPosition( allNodes->value( e )->getTargetPosition() ) ) {
				this->focused = true;

				this->focusedNode = allNodes->value( e );
				this->focusedNode->setDrawableColor( osg::Vec4( 0.0f,1.0f,0.0f,1.0f ) );
				this->focusedNode->setUsingInterpolation( false );
				this->focusedNode->setIgnoreByLayout( true );
				this->focusedNode->setTargetPosition( this->position );

				//SELECTION MODE - ONLY PICKED NODE
				/*
				for(auto j : this->focusedNode->getEdges()->keys()){
				    this->focusedNode->getEdges()->value(j)->getOtherNode( this->focusedNode )->setIgnoreByLayout( true );
				    this->focusedNode->getEdges()->value(j)->getOtherNode( this->focusedNode )->setDrawableColor( osg::Vec4( 0.0f,1.0f,0.0f,0.2f ) );
				}
				*/

				//restart kill timer
				QMetaObject::invokeMethod( this->timer, "start",Qt::QueuedConnection );

				break;
			}
		}
	}
	else {
		this->focusedNode->setTargetPosition( this->position );
		//restart kill timer
		QMetaObject::invokeMethod( this->timer, "start",Qt::QueuedConnection );
	}
}

bool ArControlObject::chckIfNearPosition( osg::Vec3f target )
{
	if ( ( this->position - target ).length() < 25.0f ) {
		return true;
	}
	return false;
}













ArControlClass::ArControlClass()
{
	viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
	coreGraph = AppCore::Core::getInstance()->getCoreGraph();
}
void ArControlClass::updateObjectPositionAruco( qlonglong object_id, QMatrix4x4 modelViewMatrix, bool reverse )
{

	osg::Matrixd markerMVM( modelViewMatrix.operator()( 0,0 ),modelViewMatrix.operator()( 0,1 ),modelViewMatrix.operator()( 0,2 ),modelViewMatrix.operator()( 0,3 ),
							modelViewMatrix.operator()( 1,0 ),modelViewMatrix.operator()( 1,1 ),modelViewMatrix.operator()( 1,2 ),modelViewMatrix.operator()( 1,3 ),
							modelViewMatrix.operator()( 2,0 ),modelViewMatrix.operator()( 2,1 ),modelViewMatrix.operator()( 2,2 ),modelViewMatrix.operator()( 2,3 ),
							modelViewMatrix.operator()( 3,0 ),modelViewMatrix.operator()( 3,1 ),modelViewMatrix.operator()( 3,2 ),modelViewMatrix.operator()( 3,3 ) );

	// transformation vector user to move graph over aruco shadow base (jurik)
	osg::Vec3f arucoBaseDist = coreGraph->getGrafRotTransVec();

	osg::Matrixd baseMVM = viewer->getCamera()->getViewMatrix();
	osg::Matrixd transMVM = markerMVM.operator *( baseMVM.inverse( baseMVM ) );
	osg::Vec3f targetPosition = transMVM.getTrans();

	// ani srnky netusia preco /2 ... ale takto to ide :D :D
	// arucoBaseDist -> translation of graph group ower the base
	// if marker is not behind, reverse coordinates
	if ( !reverse ) {
		// TO DO - nefunguje ani jurikova base ... zistit co je problem
		targetPosition.set( ( targetPosition - arucoBaseDist ).operator /( -2 ) );
	}
	else {
		//position of second marker in world coordinate system
		targetPosition.set( ( targetPosition - arucoBaseDist ).operator /( 2 ) );
	}

	if ( controlObjects.value( object_id ) != NULL ) {
		//if object is lost, destroy and create new
		if ( controlObjects.value( object_id )->isLost() ) {
			controlObjects.remove( object_id );
			controlObjects.insert( object_id, new ArControlObject( object_id, targetPosition ) );
		}
		else {
			controlObjects.value( object_id )->updatePosition( targetPosition );
		}
	}
	else {
		controlObjects.insert( object_id, new ArControlObject( object_id, targetPosition ) );
	}
}

} // namespace ArucoModul
