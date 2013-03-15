#include "Network/executors/GraphStartExecutor.h"
#include "Manager/Manager.h"
#include "Importer/GraphOperations.h"

using namespace Network;

void GraphStartExecutor::execute_client() {

    Client * client = Client::getInstance();
    client->ignoreLayout(true);

    client->currentGraph = Manager::GraphManager::getInstance()->createNewGraph("NewGraph");
    Importer::GraphOperations * operations = new Importer::GraphOperations(*client->currentGraph);
    operations->addDefaultTypes(client->edgeType, client->nodeType);

    client->thread->pause();
    client->coreGraph->setNodesFreezed(true);
    client->currentGraph->setFrozen(true);

}

void GraphStartExecutor::execute_server() {
    return;
}
