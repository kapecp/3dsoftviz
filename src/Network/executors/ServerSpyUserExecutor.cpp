#include "Network/executors/ServerSpyUserExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

using namespace Network;

ServerSpyUserExecutor::ServerSpyUserExecutor(QRegExp regex, QTcpSocket *senderClient, QString line){
    this->regexp = regex;
    this->senderClient = senderClient;
    this->line = line;
}

void ServerSpyUserExecutor::execute() {

    Server * server = Server::getInstance();

    QSet<QTcpSocket*> clients = server->getClients();

    QString message = (line+";spy:"+QString::number(server->getUserId(senderClient))+"\n");

    foreach(QTcpSocket *client, clients) { // append sender ID and resend to all other clients except sender
        if (client == senderClient) continue;
        client->write(message.toUtf8());
    }

}
