#include "Network/executors/ServerMoveAvatarExecutor.h"
#include "Network/Server.h"

using namespace Network;

ServerMoveAvatarExecutor::ServerMoveAvatarExecutor(QRegExp regex, QTcpSocket *senderClient, QString line){
    this->regexp = regex;
    this->senderClient = senderClient;
    this->line = line;
}

void ServerMoveAvatarExecutor::execute() {

    Server * server = Server::getInstance();

    QSet<QTcpSocket*> clients = server->getClients();

    foreach(QTcpSocket *client, clients) { // append sender ID and resend to all other clients except sender
        if (client == senderClient) continue;
        client->write((line+";id:"+QString::number(server->getUserId(senderClient))+"\n").toUtf8());
    }

    osg::Vec3d center = osg::Vec3d(regexp.cap(1).toFloat()-5,regexp.cap(2).toFloat(),regexp.cap(3).toFloat());
    osg::Quat rotation = osg::Quat(regexp.cap(4).toFloat(),regexp.cap(5).toFloat(),regexp.cap(6).toFloat(),regexp.cap(7).toFloat());

    osg::PositionAttitudeTransform * PAtransform = server->getAvatarTransform(senderClient);
    if (PAtransform != NULL) {
        PAtransform->setAttitude(rotation);
        PAtransform->setPosition(center);
    }

    if (server->userToSpy() == senderClient) {
        server->setMyView(center,rotation);
    }

}
