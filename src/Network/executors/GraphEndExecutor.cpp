#include "Network/executors/GraphEndExecutor.h"
#include "Network/Client.h"


using namespace Network;

void GraphEndExecutor::execute() {

    Client * client = Client::getInstance();

    client->thread->play();
    client->coreGraph->setNodesFreezed(false);
    client->currentGraph->setFrozen(false);
    client->ignoreLayout(false);

}
