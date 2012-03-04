#include "Network/executors/ServerUnspyUserExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

using namespace Network;

ServerUnspyUserExecutor::ServerUnspyUserExecutor(QRegExp regex, QTcpSocket *senderClient, QString line){
    this->regexp = regex;
    this->senderClient = senderClient;
    this->line = line;
}

void ServerUnspyUserExecutor::execute() {

    Server * server = Server::getInstance();

    QSet<QTcpSocket*> clients = server->getClients();

    QString message = (line+";spy:"+QString::number(server->getUserId(senderClient))+"\n");

    foreach(QTcpSocket *client, clients) { // append sender ID and resend to all other clients except sender
        if (client == senderClient) continue;
        client->write(message.toUtf8());
    }

    int spied = regexp.cap(1).toInt();
    if (spied == 0) {
        server->sendMyView();
    }

    server->addAvatar(senderClient, server->generateAvatar());

}
