#include "Network/executors/SpyUserExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

void SpyUserExecutor::execute() {

    Client *client = Client::getInstance();

    int spied,spy;

    *stream >> spied >> spy;

    client->removeAvatar(spy);

}
