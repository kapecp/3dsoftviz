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
#include "Viewer/PickHandler.h"


namespace ArucoModul {

/*
 * ArControlObject
 */
ArControlObject::ArControlObject( int id, osg::Vec3f position, ArAssignmentStrategy* _assignmentStrategy, ArSelectionLayoutStrategy* _selectionLayoutStrategy )
{
	this->_assignmentStrategy = _assignmentStrategy;
	this->_selectionLayoutStrategy = _selectionLayoutStrategy;

	this->id = id;
	this->position = position;
	this->focused = false;

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

	qDebug() << "asigning";

	this->focusedNode = this->_assignmentStrategy->assign( this->position );
	if ( this->focusedNode != NULL ) {
		qDebug() << "focused";

		//     AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt()->getPickHandler()->addPickedNode(this->focusedNode);
		//      AppCore::Core::getInstance()->getCoreWindow()->forceOnChange();

		this->focused = true;
		this->_selectionLayoutStrategy->setSelectionLayout( this->focusedNode );
		updatePosition( this->position );
	}
}

void ArControlObject::timerEvent()
{
	qDebug() << "LOST MARKER TRACK" ;
	this->_selectionLayoutStrategy->resetSelectionLayout( this->focusedNode );
	this->focused = false;

//   AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt()->getPickHandler()->unselectPickedNodes(this->focusedNode);
//   AppCore::Core::getInstance()->getCoreWindow()->forceOnChange();

	this->focusedNode = NULL;
}

void ArControlObject::updatePosition( osg::Vec3f position )
{
	this->position = position;
	this->focusedNode->setTargetPosition( position );

	//restart kill timer
	QMetaObject::invokeMethod( this->timer, "start",Qt::QueuedConnection );
}





/*
 * ArControlClass
 */
ArControlClass::ArControlClass()
{
	viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
	coreGraph = AppCore::Core::getInstance()->getCoreGraph();

	_assignmentStrategy = new ArAssignmentStrategyPosition();
	_selectionLayoutStrategy = new ArSelectionLayoutStrategyNodeOnly();
}
void ArControlClass::updateObjectPositionAruco( qlonglong object_id, QMatrix4x4 modelViewMatrix, bool reverse )
{
	qDebug() << "updateObjectPositionAruco";

	osg::Matrixd markerMVM( static_cast<double>( modelViewMatrix.operator()( 0,0 ) ),static_cast<double>( modelViewMatrix.operator()( 0,1 ) ),static_cast<double>( modelViewMatrix.operator()( 0,2 ) ),static_cast<double>( modelViewMatrix.operator()( 0,3 ) ),
							static_cast<double>( modelViewMatrix.operator()( 1,0 ) ),static_cast<double>( modelViewMatrix.operator()( 1,1 ) ),static_cast<double>( modelViewMatrix.operator()( 1,2 ) ),static_cast<double>( modelViewMatrix.operator()( 1,3 ) ),
							static_cast<double>( modelViewMatrix.operator()( 2,0 ) ),static_cast<double>( modelViewMatrix.operator()( 2,1 ) ),static_cast<double>( modelViewMatrix.operator()( 2,2 ) ),static_cast<double>( modelViewMatrix.operator()( 2,3 ) ),
							static_cast<double>( modelViewMatrix.operator()( 3,0 ) ),static_cast<double>( modelViewMatrix.operator()( 3,1 ) ),static_cast<double>( modelViewMatrix.operator()( 3,2 ) ),static_cast<double>( modelViewMatrix.operator()( 3,3 ) ) );

	// transformation vector user to move graph over aruco shadow base (jurik)
	osg::Vec3f arucoBaseDist = coreGraph->getGrafRotTransVec();
	osg::Vec3f graphScale = coreGraph->getGrafRotTransScale();

	osg::Matrixd baseMVM = viewer->getCamera()->getViewMatrix();
	osg::Matrixd transMVM = markerMVM.operator *( baseMVM.inverse( baseMVM ) );
	osg::Vec3f targetPosition = transMVM.getTrans();

	// ani srnky netusia preco /2 ... ale takto to ide :D :D
	// arucoBaseDist -> translation of graph group ower the base
	// if marker is not behind, reverse coordinates
	if ( !reverse ) {
		// TO DO - nefunguje ani jurikova base ... zistit co je problem
		targetPosition.set( ( targetPosition - arucoBaseDist ).operator /( -2*graphScale.x() ) );
	}
	else {
		//position of second marker in world coordinate system
		targetPosition.set( ( targetPosition - arucoBaseDist ).operator /( 2*graphScale.x() ) );
	}
	if ( controlObjects.value( object_id ) != NULL ) {
		//if object is lost, destroy and create new
		if ( !controlObjects.value( object_id )->isFocused() ) {
			qDebug() << "reinsert object";

			controlObjects.remove( object_id );

			ArControlObject* newControlObject = new ArControlObject( static_cast<int>( object_id ), targetPosition, _assignmentStrategy, _selectionLayoutStrategy );
			if ( newControlObject->isFocused() ) {
				//sucesfully assigned to graph node
				controlObjects.insert( object_id,  newControlObject );
			}

		}
		else {
			qDebug() << "update object";
			controlObjects.value( object_id )->updatePosition( targetPosition );
		}
	}
	else {
		qDebug() << "new object";
		ArControlObject* newControlObject = new ArControlObject( static_cast<int>( object_id ), targetPosition, _assignmentStrategy, _selectionLayoutStrategy );
		if ( newControlObject->isFocused() ) {
			//sucesfully assigned to graph node
			controlObjects.insert( object_id,  newControlObject );
		}

	}
}

void ArControlClass::setNodeAssignmentStrategy( int strategy )
{
	switch ( strategy ) {
		case 0 :
			_assignmentStrategy = new ArAssignmentStrategyPosition();
			break;
		case 1 :
			_assignmentStrategy = new ArAssignmentStrategyEdgeCount();
			break;
		case 2 :
			_assignmentStrategy = new ArAssignmentStrategyNearest();
			break;
	}
	qDebug() << "set" << strategy;

	// potencionalne zbytocne, kedze kazdy objekt v mape uz je assignuty
	for ( auto e : controlObjects.keys() ) {
		controlObjects.value( e )->setNodeAssignmentStrategy( _assignmentStrategy );
	}

}
void ArControlClass::setNodeBehaviourStrategy( int strategy )
{
	switch ( strategy ) {
		case 0 :
			_selectionLayoutStrategy = new ArSelectionLayoutStrategyNodeOnly();
			break;
		case 1 :
			_selectionLayoutStrategy = new ArSelectionLayoutStrategyNodeCluster();
			break;
	}
	qDebug() << "set" << strategy;

	for ( auto e : controlObjects.keys() ) {
		controlObjects.value( e )->setNodeBehaviourStrategy( _selectionLayoutStrategy );
	}
}
} // namespace ArucoModul
