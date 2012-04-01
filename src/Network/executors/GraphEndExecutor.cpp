#include "Network/executors/GraphEndExecutor.h"
#include "Network/Client.h"


using namespace Network;

void GraphEndExecutor::execute_client() {

    Client * client = Client::getInstance();

    client->thread->play();
    client->coreGraph->setNodesFreezed(false);
    client->currentGraph->setFrozen(false);
    client->ignoreLayout(false);

}

void GraphEndExecutor::execute_server() {
    return;
}
