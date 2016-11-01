#include "Network/executors/RemoveNodeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif


namespace Network {

void RemoveNodeExecutor::execute_client()
{

	int id;

	*stream >> id;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	if ( nodes->contains( id ) ) {
		Data::Node* node = *nodes->find( id );
		currentGraph->removeNode( node );
	}
	else {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getMetaNodes();
		if ( nodes->contains( id ) ) {
			Data::Node* node = *nodes->find( id );
			currentGraph->removeNode( node );
		}
		else {
			qDebug() << "Client: delete node: unknown node" << id;
		}
	}

}

void RemoveNodeExecutor::execute_server()
{

	int id;

	*stream >> id;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	if ( nodes->contains( id ) ) {
		Data::Node* node = *nodes->find( id );
		currentGraph->removeNode( node );
	}
	else {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getMetaNodes();
		if ( nodes->contains( id ) ) {
			Data::Node* node = *nodes->find( id );
			currentGraph->removeNode( node );
		}
		else {
			qDebug() << "Server: delete node: unknown node" << id;
		}
	}

	Server* server = Server::getInstance();
	server->sendRemoveNode( id );

	if ( ( ( QOSG::CoreWindow* )server->getCoreWindowReference() )->playing() ) {
		server->getLayoutThread()->play();
	}

}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
