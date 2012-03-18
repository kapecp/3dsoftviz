#include "Network/executors/ServerSpyUserExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

using namespace Network;

void ServerSpyUserExecutor::execute() {

    Server * server = Server::getInstance();
    QTcpSocket * senderClient = (QTcpSocket*)stream->device();

    QSet<QTcpSocket*> clients = server->getClients();

    int spying;
    *stream >> spying;

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    out << (quint16)0 << SpyUserExecutor::INSTRUCTION_NUMBER << spying << (int) server->getUserId(senderClient);

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    foreach(QTcpSocket *client, clients) { // append sender ID and resend to all other clients except sender
        if (client == senderClient) continue;
        client->write(block);
    }

    server->removeAvatar(senderClient);

}
