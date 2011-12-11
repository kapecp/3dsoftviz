#include "Network/executors/ServerSendGraphExecutor.h"
#include "Network/Server.h"

using namespace Network;

ServerSendGraphExecutor::ServerSendGraphExecutor(QTcpSocket * senderClient){
    this->senderClient = senderClient;
}

void ServerSendGraphExecutor::execute() {

    Server *server = Server::getInstance();
    server->sendGraph(senderClient);

}
