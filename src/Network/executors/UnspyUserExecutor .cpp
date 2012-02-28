#include "Network/executors/UnspyUserExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

UnspyUserExecutor::UnspyUserExecutor(QRegExp regex){
    this->regexp = regex;
}

void UnspyUserExecutor::execute() {

    Client *client = Client::getInstance();

    int spied = regexp.cap(1).toInt();
    int spy = regexp.cap(2).toInt();

    client->showClientAvatar(spy);

}
