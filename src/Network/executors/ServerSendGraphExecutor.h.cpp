#include "Network/executors/ServerSendGraphExecutor.h"
#include "Network/Server.h"

using namespace Network;


void ServerSendGraphExecutor::execute() {

    Server *server = Server::getInstance();
    QTcpSocket * senderClient = (QTcpSocket *) stream->device();
    server->sendGraph(senderClient);

}
