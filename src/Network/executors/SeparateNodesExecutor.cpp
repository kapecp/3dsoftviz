#include "Network/executors/SeparateNodesExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"
#include "Network/Server.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wsign-conversion"

namespace Network {

void SeparateNodesExecutor::execute_client()
{

	int count, id;

	*stream >>count;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getMetaNodes();

	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = new QLinkedList<osg::ref_ptr<Data::Node> >();

	for ( int i = 0; i < count; i++ ) {
		*stream >> id;
		if ( nodes->contains( id ) ) {
			selectedNodes->append( *nodes->find( id ) );
		}
	}
	currentGraph->separateNodes( selectedNodes );

}

void SeparateNodesExecutor::execute_server()
{

	int count, id;

	*stream >>count;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getMetaNodes();

	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = new QLinkedList<osg::ref_ptr<Data::Node> >();

	for ( int i = 0; i < count; i++ ) {
		*stream >> id;
		if ( nodes->contains( id ) ) {
			selectedNodes->append( *nodes->find( id ) );
		}
	}

	Server* server = Server::getInstance();
	currentGraph->separateNodes( selectedNodes );

	server->sendSeparateNodes( selectedNodes );

	if ( ( ( QOSG::CoreWindow* )server->getCoreWindowReference() )->playing() ) {
		server->getLayoutThread()->play();
	}
	return;

}

} // namespace Network

#pragma GCC diagnostic pop
