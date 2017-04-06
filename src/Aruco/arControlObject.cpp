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

    //assignNodeByPosition();
    //assignNodeByMatric();
    assignNodeByEdgeCount();
}

void ArControlObject::timerEvent()
{
	qDebug() << "LOST MARKER TRACK" ;
    doUnAssignNode( this->focusedNode );
}

void ArControlObject::updatePosition( osg::Vec3f position )
{
	this->position = position;
    this->focusedNode->setTargetPosition( position );
    //restart kill timer
    QMetaObject::invokeMethod( this->timer, "start",Qt::QueuedConnection );
}

void ArControlObject::doAssignNode( osg::ref_ptr<Data::Node> node){
    this->focused = true;
    this->focusedNode = node;

    node->setDrawableColor( osg::Vec4( 0.0f,1.0f,0.0f,1.0f ) );
    node->setUsingInterpolation( false );
    node->setIgnoreByLayout( true );
    node->setTargetPosition( this->position );

    //SELECTION MODE - ONLY PICKED NODE
    /*
    for(auto j : node->getEdges()->keys()){
        node->getEdges()->value(j)->getOtherNode( node )->setIgnoreByLayout( true );
        node->getEdges()->value(j)->getOtherNode( node )->setDrawableColor( osg::Vec4( 0.0f,1.0f,0.0f,0.2f ) );
    }
    */
}

void ArControlObject::doUnAssignNode( osg::ref_ptr<Data::Node> node ){
    this->focused = false;
    this->focusedNode = NULL;

    node->setDefaultColor();
    node->setUsingInterpolation( true );
    node->setIgnoreByLayout( false );

    //SELECTION MODE - ONLY PICKED NODE
    /*
    for(auto j : this->focusedNode->getEdges()->keys()){
        node->getEdges()->value(j)->getOtherNode( node )->setIgnoreByLayout( false );
        node->getEdges()->value(j)->getOtherNode( node )->setDefaultColor();
    }
    */
}

bool ArControlObject::assignNodeByPosition()
{
    Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

    for ( auto e : allNodes->keys() ) {
        // not already used and marker near node, pick it
        if ( !allNodes->value( e )->isIgnoredByLayout() && chckIfNearPosition( allNodes->value( e )->getTargetPosition() ) ) {
            doAssignNode( allNodes->value( e ) );
            updatePosition( this->position );
            return true;
        }
    }
    return false;
}

bool ArControlObject::assignNodeByMatric()
{
    return false;
}

bool ArControlObject::assignNodeByEdgeCount()
{
    int most_edges_count = 0;
    osg::ref_ptr<Data::Node> most_edges_node_ref = NULL;

    Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

    for ( auto e : allNodes->keys() ) {
        if ( !allNodes->value( e )->isIgnoredByLayout() && allNodes->value( e )->getEdges()->count() > most_edges_count ) {
           most_edges_count = allNodes->value( e )->getEdges()->count();
           most_edges_node_ref = allNodes->value( e );
        }
    }

    if(most_edges_node_ref != NULL){
        doAssignNode( most_edges_node_ref );
        updatePosition( this->position );
        return true;
    }
    else{
        qDebug() << "Unable to assign more nodes...";
        return false;
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
        if ( !controlObjects.value( object_id )->isFocused() ) {
			controlObjects.remove( object_id );

            ArControlObject* newControlObject = new ArControlObject( object_id, targetPosition );
            if(newControlObject->isFocused()){
                //sucesfully assigned to graph node
                controlObjects.insert( object_id,  newControlObject);
            }

		}
		else {
			controlObjects.value( object_id )->updatePosition( targetPosition );
		}
	}
	else {

        ArControlObject* newControlObject = new ArControlObject( object_id, targetPosition );
        if(newControlObject->isFocused()){
            //sucesfully assigned to graph node
            controlObjects.insert( object_id,  newControlObject);
        }

	}
}

} // namespace ArucoModul
