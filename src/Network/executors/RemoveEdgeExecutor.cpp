#include "Network/executors/RemoveEdgeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void RemoveEdgeExecutor::execute_client()
{

	int id;

	*stream >> id;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges = currentGraph -> getEdges();
	if ( edges->contains( id ) ) {
		Data::Edge* edge = *edges->find( id );
		if ( edge != NULL ) {
			currentGraph->removeEdge( edge );
		}
	}

}

void RemoveEdgeExecutor::execute_server()
{

	int id;

	*stream >> id;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges = currentGraph -> getEdges();
	if ( edges->contains( id ) ) {
		Data::Edge* edge = *edges->find( id );
		if ( edge != NULL ) {
			currentGraph->removeEdge( edge );
		}
	}

	Server* server = Server::getInstance();
	server->sendRemoveEdge( id );

	if ( ( ( QOSG::CoreWindow* )server->getCoreWindowReference() )->playing() ) {
		server->getLayoutThread()->play();
	}

}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
