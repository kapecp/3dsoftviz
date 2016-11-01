#include "Network/executors/SetNodeColorExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void SetNodeColorExecutor::execute_client()
{
	int id;

	float r, g, b, alpha;

	*stream >> id >> r >> g >> b >> alpha;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	if ( nodes->contains( id ) ) {
		Data::Node* node = *nodes->find( id );
		node->setColor( osg::Vec4( r,g,b,alpha ) );
	}

}

void SetNodeColorExecutor::execute_server()
{
	int id;
	float r, g, b, alpha;

	*stream >> id >> r >> g >> b >> alpha;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	if ( nodes->contains( id ) ) {
		Data::Node* node = *nodes->find( id );
		node->setColor( osg::Vec4( r,g,b,alpha ) );
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
