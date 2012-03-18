#include "Network/executors/UnspyUserExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

void UnspyUserExecutor::execute() {

    Client *client = Client::getInstance();

    int spied,spy;
    *stream >> spied >> spy;

    if (spied == client->getMyId()) {
        client->sendMyView();
    }

    client->showClientAvatar(spy);

}
