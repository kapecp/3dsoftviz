#include "Network/executors/SendGraphExecutor.h"
#include "Network/Server.h"

using namespace Network;


void SendGraphExecutor::execute_server() {

    Server *server = Server::getInstance();
    QTcpSocket * senderClient = (QTcpSocket *) stream->device();
    server->sendGraph(senderClient);

}

void SendGraphExecutor::execute_client() {
    return;
}
