#include "Network/executors/SetFixNodeStateExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"
#include "Network/Server.h"
#include "Data/Node.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void SetFixNodeStateExecutor::execute_client()
{
	int id;
	bool state;

	*stream >> id >> state;


	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	Client* client = Client::getInstance();
	if ( nodes->contains( id ) ) {
		Data::Node* node = *nodes->find( id );
		this->SetFixNodeState( node, state );
		if ( client->selected_nodes.contains( node ) ) {
			client->selected_nodes.removeOne( node );
		}
	}
	else {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* mergeNodes = currentGraph->getMetaNodes();
		if ( mergeNodes->contains( id ) ) {
			Data::Node* node = *mergeNodes->find( id );
			this->SetFixNodeState( node,state );
			if ( client->selected_nodes.contains( node ) ) {
				client->selected_nodes.removeOne( node );
			}
		}
	}

}

void SetFixNodeStateExecutor::execute_server()
{
	int id;
	bool state;

	*stream >> id >> state;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	if ( nodes->contains( id ) ) {
		this->SetFixNodeState( *nodes->find( id ), state );
	}
	else {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* mergeNodes = currentGraph->getMetaNodes();
		if ( mergeNodes->contains( id ) ) {
			this->SetFixNodeState( *mergeNodes->find( id ),state );
		}
	}

	Server* server = Server::getInstance();
	server->sendFixNodeState( id, state );

	if ( ( ( QOSG::CoreWindow* )server->getCoreWindowReference() )->playing() ) {
		server->getLayoutThread()->play();
	}

}

void SetFixNodeStateExecutor::SetFixNodeState( Data::Node* node, bool state )
{

	node->setFixed( state );
	node->setUsingInterpolation( !state );

}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
