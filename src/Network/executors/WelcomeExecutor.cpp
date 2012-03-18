#include "Network/executors/WelcomeExecutor.h"
#include "Network/Client.h"

using namespace Network;

void WelcomeExecutor::execute() {

    Client * client = Client::getInstance();
    int id;
    *stream >> id;
    client->setMyId(id);

}
