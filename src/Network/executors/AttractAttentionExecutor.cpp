#include "Network/executors/AttractAttentionExecutor.h"
#include "Network/Server.h"

using namespace Network;

void AttractAttentionExecutor::execute_client() {

    bool attract;
    int id_user;

    *stream >> attract >> id_user;

    Client * client= Client::getInstance();

    if (attract) {
        client->setAttention(id_user);
    } else {
        client->unSetAttention(id_user);
    }
}

void AttractAttentionExecutor::execute_server() {

    QTcpSocket * sender = (QTcpSocket*) stream->device();
    bool attract;
    *stream >> attract;

    Server * server = Server::getInstance();
    int id = server->getUserId(sender);

    if (attract) {
        server->setAttention(id);
    } else {
        server->unSetAttention(id);
    }

    QSet<QTcpSocket*> clients = server->getClients();
    foreach (QTcpSocket *client, clients) {
        if (client == sender) {
            continue;
        }
        server->sendAttractAttention(attract,id,client);
    }

}
