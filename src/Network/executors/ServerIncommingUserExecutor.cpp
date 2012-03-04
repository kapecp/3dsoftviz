#include "Network/executors/ServerIncommingUserExecutor.h"
#include "Network/Server.h"

using namespace Network;

ServerIncommingUserExecutor::ServerIncommingUserExecutor(QRegExp regexp, QTcpSocket * senderClient){
    this->regexp = regexp;
    this->senderClient = senderClient;
}

void ServerIncommingUserExecutor::execute() {

    QString user = regexp.cap(1);

    Server * server = Server::getInstance();

    int newID = 1;
    if (server->getUserCount() > 0) {
        newID = server->getMaxUserId() + 1;
    }
    server->addUser(senderClient, user, newID);

    osg::PositionAttitudeTransform* PAtransform = server->generateAvatar();

    server->addAvatar(senderClient, PAtransform);

    senderClient->write(("/yourid:"+QString::number(newID)+"\n").toUtf8());

    server->updateUserList();
    server->sendUserList();
    server->sendMyView(senderClient);

}
