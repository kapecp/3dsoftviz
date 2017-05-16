#include "Network/executors/GraphStartExecutor.h"
#include "Manager/Manager.h"
#include "Importer/GraphOperations.h"
#include "Network/Client.h"

namespace Network {

void GraphStartExecutor::execute_client()
{

	Client* client = Client::getInstance();
	client->ignoreLayout( true );

	client->currentGraph = Manager::GraphManager::getInstance()->createNewGraph( "NewGraph" );
	Importer::GraphOperations* operations = new Importer::GraphOperations( *client->currentGraph );
	operations->addDefaultTypes( client->edgeType, client->nodeType );

	client->thread->pauseAllAlg();
	client->coreGraph->setNodesFreezed( true );
	client->currentGraph->setFrozen( true );
	delete operations;
}

void GraphStartExecutor::execute_server()
{
	return;
}

} // namespace Network
