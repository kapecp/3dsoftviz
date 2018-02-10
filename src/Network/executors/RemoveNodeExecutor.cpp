#include "Network/executors/RemoveNodeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

#include <leathers/push>
#include <leathers/useless-cast>
#include <leathers/old-style-cast>
#include <leathers/sign-conversion>

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

#include <leathers/pop>
