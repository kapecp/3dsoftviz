#include "Network/executors/ServerStopExecutor.h"
#include "Network/Client.h"

using namespace Network;

void ServerStopExecutor::execute() {

    Client *client = Client::getInstance();
    client->disconnect();

}
