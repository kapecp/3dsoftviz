#include "Network/executors/ServerIncommingUserExecutor.h"
#include "Network/executors/WelcomeExecutor.h"
#include "Network/Helper.h"
#include "Network/Server.h"

using namespace Network;

void ServerIncommingUserExecutor::execute() {

    QString user;
    *stream >> user;

    QTcpSocket * out_socket = (QTcpSocket*) stream->device();

    Server * server = Server::getInstance();

    int newID = 1;
    if (server->getUserCount() > 0) {
        newID = server->getMaxUserId() + 1;
    }
    server->addUser(out_socket, user, newID);

    server->addAvatar(out_socket, user);

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << (quint16)0;
    out << WelcomeExecutor::INSTRUCTION_NUMBER;
    out << newID;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    out_socket->write(block);

    server->updateUserList();
    server->sendUserList();
    server->sendMyView(out_socket);

}
