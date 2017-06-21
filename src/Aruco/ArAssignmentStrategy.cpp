#include "Aruco/ArAssignmentStrategy.h"
#include <QDebug>
#include <QMap>

#include "Manager/Manager.h"
#include "Data/Graph.h"
#include "Data/Node.h"


namespace ArucoModul {

/*
 * ArAssignmentStrategy
 */
ArAssignmentStrategy::ArAssignmentStrategy()
{

}

osg::ref_ptr<Data::Node> ArAssignmentStrategy::assign( osg::Vec3f position )
{
	return pickNodeToAssign( position );
}


/*
 * ArAssignmentStrategyPosition
 */
osg::ref_ptr<Data::Node> ArAssignmentStrategyPosition::pickNodeToAssign( osg::Vec3f position )
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

	for ( auto e : allNodes->keys() ) {
		// not already used and marker near node, pick it
		if ( !allNodes->value( e )->isIgnoredByLayout() && chckIfNearPosition( position, allNodes->value( e )->getTargetPosition() ) ) {
			return allNodes->value( e );
		}
	}
	return NULL;
}

bool ArAssignmentStrategyPosition::chckIfNearPosition( osg::Vec3f source, osg::Vec3f target )
{
	if ( ( source - target ).length() < 25.0f ) {
		return true;
	}
	return false;
}






/*
 * ArAssignmentStrategyEdgeCount
 */
osg::ref_ptr<Data::Node> ArAssignmentStrategyEdgeCount::pickNodeToAssign( osg::Vec3f position )
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

	if ( most_edges_node_ref != NULL ) {
		return most_edges_node_ref;
	}
	else {
		qDebug() << "Unable to assign more nodes...";
		return NULL;
	}
}





/*
 * ArAssignmentStrategyNearest
 */
osg::ref_ptr<Data::Node> ArAssignmentStrategyNearest::pickNodeToAssign( osg::Vec3f position )
{
	float min_node_distace = 999999;
	osg::ref_ptr<Data::Node> most_nearest_node_ref = NULL;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

	for ( auto e : allNodes->keys() ) {
		float node_distance = ( position -  allNodes->value( e )->getTargetPosition() ).length();
		if ( !allNodes->value( e )->isIgnoredByLayout() && node_distance < min_node_distace ) {
			min_node_distace = node_distance;
			most_nearest_node_ref = allNodes->value( e );
		}
	}

	if ( most_nearest_node_ref != NULL ) {
		return most_nearest_node_ref;
	}
	else {
		qDebug() << "Unable to assign more nodes...";
		return NULL;
	}
}

}
