#include "Network/executors/AddMetaNodeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"
#include "Network/Server.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void AddMetaNodeExecutor::execute_client()
{

	int count, metaNodeId, id_edge, id_node;
	QString name, edgeName;
	float x, y, z;

	*stream >> metaNodeId >> name;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();

	*stream >> x >> y >> z;

	osg::ref_ptr<Data::Node> metaNode = currentGraph->addNode( metaNodeId, name, currentGraph->getNodeMetaType(), osg::Vec3( x,y,z ) );

	*stream >> edgeName;
	*stream >> count;
	for ( int i = 0; i < count; i++ ) {
		*stream >> id_edge >> id_node;
		if ( nodes->contains( id_node ) ) {
			currentGraph->addEdge( id_edge, edgeName, *nodes->find( id_node ), metaNode, currentGraph->getEdgeMetaType(), true );
		}
	}

}

void AddMetaNodeExecutor::execute_server()
{

	int count, id;
	QString name, edgeName;
	float x, y, z;

	*stream >> name;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();

	*stream >> x >> y >> z;

	osg::ref_ptr<Data::Node> metaNode = currentGraph->addNode( name, currentGraph->getNodeMetaType(), osg::Vec3( x,y,z ) );

	*stream >> edgeName;
	*stream >> count;
	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = new QLinkedList<osg::ref_ptr<Data::Node> >();

	for ( int i = 0; i < count; i++ ) {
		*stream >> id;
		if ( nodes->contains( id ) ) {
			currentGraph->addEdge( edgeName, *nodes->find( id ), metaNode, currentGraph->getEdgeMetaType(), true );
			selectedNodes->append( *nodes->find( id ) );
		}
	}

	Server* server = Server::getInstance();
	server->sendAddMetaNode( metaNode,selectedNodes,edgeName,osg::Vec3( x,y,z ) );

	if ( ( ( QOSG::CoreWindow* )server->getCoreWindowReference() )->playing() ) {
		server->getLayoutThread()->play();
	}


}

}

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
