/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Server.h"
#include "Network/Helper.h"
#include "Data/Graph.h"
#include "Manager/Manager.h"

#include <QTcpSocket>
#include <QRegExp>

using namespace Network;

Server * Server::instance;

Server::Server(QObject *parent) : QTcpServer(parent)
{
    instance = this;
    cw = parent;
    Util::ApplicationConfig *conf = Util::ApplicationConfig::get();
    graphScale = conf->getValue("Viewer.Display.NodeDistanceScale").toFloat();
    executorFactory = new ExecutorFactory();
    user_to_spy = NULL;
    user_to_center = NULL;

    blockSize = 0;
}

Server* Server::getInstance() {
    if (instance == NULL) {
        return new Server();
    } else {
        return instance;
    }
}

void Server::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clients.insert(client);

    qDebug() << "New client from:" << client->peerAddress().toString();
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
}

void Server::readyRead()
{
    QTcpSocket *senderClient = (QTcpSocket*)sender();
    while(senderClient->bytesAvailable()) {
        QDataStream in(senderClient);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);

        if (blockSize == 0) {
            if (senderClient->bytesAvailable() < (int)sizeof(quint16))
                return;

            in >> blockSize;
        }

        if (senderClient->bytesAvailable() < blockSize)
            return;

        AbstractExecutor *executor = executorFactory->getExecutor(&in);

        if (executor != NULL) {
            executor->execute();
        } else {
            qDebug() << "Klient: neznama instrukcia";
        }

        blockSize = 0;
    }
}

void Server::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clients.remove(client);
    users.remove(client);
    removeAvatar(client);

    updateUserList();
    sendUserList();
}

void Server::sendUserList()
{
    QStringList userList;

    QMap<QTcpSocket*,QString>::const_iterator i;

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    foreach(QTcpSocket *client, clients){
        userList.clear();
        userList << "0=server";
        for (i = users.constBegin(); i != users.constEnd(); ++i){
            if (client == i.key()) continue;
            userList << QString::number(usersID[i.key()]) + "=" + i.value();
        }
        QString userListJoined = userList.join(",");

        block.clear();
        out.device()->reset();

        out << (quint16)0 << UsersExecutor::INSTRUCTION_NUMBER << userListJoined;
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        client->write(block);
    }
}

void Server::sendGraph(QTcpSocket *client){

    if (!this -> isListening() || (client == NULL && clients.size() == 0)) {
        return;
    }

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator iNodes =  nodes->constBegin();

    /*QTime t;
    t.start();*/

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    out << (quint16)0 << GraphStartExecutor::INSTRUCTION_NUMBER;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    if (client == NULL){
        foreach(QTcpSocket *otherClient, clients){
            otherClient->write(block);
        }
    } else {
        client -> write(block);
    }

    while(iNodes != nodes->constEnd()) {

        block.clear();
        out.device()->reset();

        out     << (quint16)0 << NewNodeExecutor::INSTRUCTION_NUMBER
                << (int) iNodes.value()->getId()
                << (float) (iNodes.value()->getCurrentPosition().x()/graphScale)
                << (float) (iNodes.value()->getCurrentPosition().y()/graphScale)
                << (float) (iNodes.value()->getCurrentPosition().z()/graphScale);

        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        if (client == NULL){
            foreach(QTcpSocket *otherClient, clients){
                otherClient->write(block);
            }
        } else {
            client->write(block);
        }

        ++iNodes;
    }

    QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges = currentGraph -> getEdges();
    QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iEdges =  edges->constBegin();

    while (iEdges != edges -> constEnd()) {

        block.clear();
        out.device()->reset();

        out    << (quint16)0 << NewEdgeExecutor::INSTRUCTION_NUMBER
               << (int) iEdges.value()->getId()
               << (int) (iEdges.value()->getSrcNode()->getId())
               << (int) (iEdges.value()->getDstNode()->getId())
               << (bool) iEdges.value()->isOriented();

        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        if (client == NULL){
            foreach(QTcpSocket *otherClient, clients){
                otherClient->write(block);
            }
        } else {
            client -> write(block);
        }

        ++iEdges;
    }

    block.clear();
    out.device()->reset();

    out << (quint16)0 << GraphEndExecutor::INSTRUCTION_NUMBER;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    if (client == NULL){
        foreach(QTcpSocket *otherClient, clients){
            otherClient->write(block);
        }
    } else {
        client -> write(block);
    }

    //qDebug() << "Sending took" << t.elapsed() << "ms";

}

void Server::sendLayout(QTcpSocket *client){

    if (!this -> isListening() || (client == NULL && clients.size() == 0)) {
        return;
    }

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

    if (currentGraph == NULL) {
        return;
    }

    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator iNodes =  nodes->constBegin();


    /*QTime t;
    t.start();*/

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    while(iNodes != nodes->constEnd()) {

        block.clear();
        out.device()->reset();

        out << (quint16)0 << LayoutExecutor::INSTRUCTION_NUMBER
            << (int) iNodes.value()->getId()
            << (float) (iNodes.value()->getCurrentPosition().x()/graphScale)
            << (float) (iNodes.value()->getCurrentPosition().y()/graphScale)
            << (float) (iNodes.value()->getCurrentPosition().z()/graphScale);

        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        if (client == NULL){
            foreach(QTcpSocket *otherClient, clients){
                otherClient->write(block);
            }
        } else {
            client -> write(block);
        }

        ++iNodes;
    }
    //qDebug() << "Sending layout took" << t.elapsed() << "ms";

}

void Server::setLayoutThread(Layout::LayoutThread *layoutThread){
    thread = layoutThread;
}

void Server::stopServer(){

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    out << (quint16)0 << ServerStopExecutor::INSTRUCTION_NUMBER;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    foreach(QTcpSocket *otherClient, clients){
        otherClient->write(block);
    }

    clients.clear();
    users.clear();

    this->close();

}

void Server::sendMoveNodes() {
    QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selected_nodes.constBegin();

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    while (i != selected_nodes.constEnd())
    {

        block.clear();
        out.device()->reset();

        out << (quint16)0 << MoveNodeExecutor::INSTRUCTION_NUMBER
            << (int) ((*i)->getId())
            << (float) ((*i)->getCurrentPosition().x())
            << (float) ((*i)->getCurrentPosition().y())
            << (float) ((*i)->getCurrentPosition().z());
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        ++i;

        foreach(QTcpSocket *otherClient, clients){
            otherClient->write(block);
        }
    }

    selected_nodes.clear();
}


void Server::sendMyView(osg::Vec3d center, osg::Quat rotation, float distance, QTcpSocket * client) {

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    out << (quint16)0 << MoveAvatarExecutor::INSTRUCTION_NUMBER << (float)center.x() << (float)center.y() << (float)center.z()
        << (float)rotation.x() << (float)rotation.y() << (float)rotation.z() << (float)rotation.w() << (float)distance << (int)0;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    if (client == NULL) {
        foreach(QTcpSocket *otherClient, clients){
            otherClient->write(block);
        }
    } else {
        client->write(block);
    }

}

void Server::sendMyView(QTcpSocket *client) {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    this->sendMyView(cameraManipulator->getCenter(), cameraManipulator->getRotation(), cameraManipulator->getDistance(), client);
}

void Server::sendMyView() {
    this->sendMyView(NULL);
}

void Server::appendMovingNode(osg::ref_ptr<Data::Node> node) {
    moving_nodes.append(node);
}

int Server::getMaxUserId() {
    QList<int> IDs = usersID.values();
    return *(std::max_element(IDs.begin(),IDs.end()));
}

void Server::updateUserList() {
    QListWidgetItem * item;
    QListWidget * lw = ((QOSG::CoreWindow *) cw)->lw_users;
    lw->clear();
    QMap<QTcpSocket*,QString>::iterator i = users.begin();
    while (i != users.end()) {
        item = new QListWidgetItem();
        item->setData(6,usersID[i.key()]);
        item->setText(i.value());
        lw->addItem(item);
        i++;
    }
}

void Server::removeAvatar(QTcpSocket *client) {

    osg::PositionAttitudeTransform *pat = avatars.take(client);
    if (pat != NULL) {
        pat->removeChildren(0,2);
    }
}

QTcpSocket * Server::getClientById(int id) {
    QMap<QTcpSocket*,int>::iterator i = usersID.begin();
    while (i != usersID.end()) {
        if (i.value() == id) {
            return i.key();
        }
        i++;
    }
    return NULL;
}

void Server::spyUser(int id) {
    user_to_spy = getClientById(id);

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    out << (quint16)0 << SpyUserExecutor::INSTRUCTION_NUMBER << id << (int)0;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    foreach(QTcpSocket *client, clients) {
        client->write(block);
    }

    // store original view
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    original_center = cameraManipulator->getCenter();
    original_rotation = cameraManipulator->getRotation();
    original_distance = cameraManipulator->getDistance();

    setMyView(avatars[user_to_spy]->getPosition(),avatars[user_to_spy]->getAttitude(),original_distance);

    removeAvatar(user_to_spy);
}

void Server::unSpyUser() {

    addAvatar(user_to_spy, getUserName(user_to_spy));

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    out << (quint16)0 << UnspyUserExecutor::INSTRUCTION_NUMBER << (int)getUserId(user_to_spy) << (int)0;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    foreach(QTcpSocket *client, clients) {
        client->write(block);
    }

    // restore original view
    setMyView(original_center,original_rotation,original_distance);

    sendMyView();

    user_to_spy = NULL;
}

void Server::addAvatar(QTcpSocket *socket, QString nick) {
    osg::PositionAttitudeTransform * avatar = Helper::generateAvatar(nick);

    QLinkedList<osg::ref_ptr<osg::Node> > * nodes = coreGraph->getCustomNodeList();
    nodes->append(avatar);

    avatars[socket] = avatar;
}

void Server::setMyView(osg::Vec3d center, osg::Quat rotation, float distance) {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    cameraManipulator->setCenter(center);
    cameraManipulator->setRotation(rotation);
    cameraManipulator->setDistance(distance);
}

void Server::lookAt(osg::Vec3d coord) {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    osg::Quat rotation = Helper::lookAt(cameraManipulator->getCenter(), coord);
    cameraManipulator->setRotation(rotation);
    sendMyView();
}

void Server::centerUser(int id_user) {
    user_to_center = getClientById(id_user);
    osg::PositionAttitudeTransform * userAvatar = avatars[user_to_center];
    lookAt(userAvatar->getPosition());
}
