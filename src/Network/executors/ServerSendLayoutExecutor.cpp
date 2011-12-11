#include "Network/executors/ServerSendLayoutExecutor.h"
#include "Network/Server.h"

using namespace Network;

ServerSendLayoutExecutor::ServerSendLayoutExecutor(QTcpSocket * senderClient){
    this->senderClient = senderClient;
}

void ServerSendLayoutExecutor::execute() {

    Server *server = Server::getInstance();
    server->sendLayout(senderClient);

}
