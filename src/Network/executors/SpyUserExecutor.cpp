#include "Network/executors/SpyUserExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

SpyUserExecutor::SpyUserExecutor(QRegExp regex){
    this->regexp = regex;
}

void SpyUserExecutor::execute() {

    Client *client = Client::getInstance();

    int spied = regexp.cap(1).toInt();
    int spy = regexp.cap(2).toInt();

    client->hideClientAvatar(spy);

}
