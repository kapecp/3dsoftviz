#include "Network/executors/SetEdgeColorExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void SetEdgeColorExecutor::execute_client()
{
	int id;

	float r, g, b, alpha;

	*stream >> id >> r >> g >> b >> alpha;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges = currentGraph -> getEdges();
	if ( edges->contains( id ) ) {
		Data::Edge* edge = *edges->find( id );
		edge->setEdgeColor( osg::Vec4( r,g,b,alpha ) );
	}

}

void SetEdgeColorExecutor::execute_server()
{
	int id;
	float r, g, b, alpha;

	*stream >> id >> r >> g >> b >> alpha;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges = currentGraph -> getEdges();
	if ( edges->contains( id ) ) {
		Data::Edge* edge = *edges->find( id );
		edge->setEdgeColor( osg::Vec4( r,g,b,alpha ) );
	}

	Server* server = Server::getInstance();
	server->sendNodeColor( id, r, g, b, alpha );

	if ( ( ( QOSG::CoreWindow* )server->getCoreWindowReference() )->playing() ) {
		server->getLayoutThread()->play();
	}

}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
