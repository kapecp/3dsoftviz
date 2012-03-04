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

    osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("avatar.osg");
    if (!modelNode) {
        qDebug() << "could not find model";
        return;
    }

    osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
    PAtransform->addChild(modelNode);

    Vwr::CoreGraph * coreGraph = server->getCoreGraph();
    QLinkedList<osg::ref_ptr<osg::Node> > * nodes = coreGraph->getCustomNodeList();

    nodes->append(PAtransform);

    //PAtransform->setScale(osg::Vec3d(10,10,10));

    server->addAvatar(senderClient, PAtransform);

    qDebug() << "posielam" << ("/yourid:"+QString::number(newID)+"\n").toUtf8();
    senderClient->write(("/yourid:"+QString::number(newID)+"\n").toUtf8());

    server->updateUserList();
    server->sendUserList();
    server->sendMyView(senderClient);

}
