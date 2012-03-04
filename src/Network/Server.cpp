/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Server.h"
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
    executorFactory = new ExecutorFactory(this);
    user_to_spy = NULL;
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
    while(senderClient->canReadLine())
    {
        QString line = QString::fromUtf8(senderClient->readLine()).trimmed();

        executorFactory->setSenderClient(senderClient);
        AbstractExecutor *executor = executorFactory->getExecutor(line);

        if (executor != NULL) {
            executor->execute();
        } else {
            qDebug() << "Server: neznama instrukcia:" << line;
        }

        /*
                tento kod sa da vyuzit pre implementaciu chatu

        QString message = line;
        QString user = users[senderClient];
        foreach(QTcpSocket *otherClient, clients)
            otherClient->write(QString(user + ":" + message + "\n").toUtf8());

        */

    }
}

void Server::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clients.remove(client);
    users.remove(client);
    avatars[client]->removeChild(0,1);
    avatars.remove(client);

    updateUserList();
    sendUserList();
}

void Server::sendUserList()
{
    QStringList userList;

    QMap<QTcpSocket*,QString>::const_iterator i;

    foreach(QTcpSocket *client, clients){
        userList.clear();
        userList << "0=server";
        for (i = users.constBegin(); i != users.constEnd(); ++i){
            if (client == i.key()) continue;
            userList << QString::number(usersID[i.key()]) + "=" + i.value();
        }
        client->write(QString("/clients:" + userList.join(",") + "\n").toUtf8());
    }
}

void Server::sendGraph(QTcpSocket *client){

    if (!this -> isListening() || (client == NULL && clients.size() == 0)) {
        return;
    }

    QString message;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator iNodes =  nodes->constBegin();

    /*QTime t;
    t.start();*/

    if (client == NULL){
        foreach(QTcpSocket *otherClient, clients){
            otherClient->write("GRAPH_START\n");
        }
    } else {
        client -> write("GRAPH_START\n");
    }

    while(iNodes != nodes->constEnd()) {

        message = "id:" + QString::number(iNodes.value()->getId());
        message += ";x:" + QString::number(iNodes.value()->getCurrentPosition().x()/graphScale);
        message += ";y:" + QString::number(iNodes.value()->getCurrentPosition().y()/graphScale);
        message += ";z:" + QString::number(iNodes.value()->getCurrentPosition().z()/graphScale);

        if (client == NULL){
            foreach(QTcpSocket *otherClient, clients){
                otherClient->write(("/nodeData:"+message+"\n").toUtf8());
            }
        } else {
            client -> write(("/nodeData:"+message+"\n").toUtf8());
        }

        //qDebug() << "[SERVER] Sending node: " << message;

        ++iNodes;
    }

    QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges = currentGraph -> getEdges();
    QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iEdges =  edges->constBegin();

    while (iEdges != edges -> constEnd()) {
        message = "id:" + QString::number(iEdges.value()->getId());
        //message += ";name:" + iEdges.value()->getName();
        message += ";from:" + QString::number(iEdges.value()->getSrcNode()->getId());
        message += ";to:" + QString::number(iEdges.value()->getDstNode()->getId());
        message += ";or:" + QString::number(iEdges.value()->isOriented() ? 1 : 0);

        if (client == NULL){
            foreach(QTcpSocket *otherClient, clients){
                otherClient->write(("/edgeData:"+message+"\n").toUtf8());
            }
        } else {
            client -> write(("/edgeData:"+message+"\n").toUtf8());
        }

        //qDebug() << "[SERVER] Sending edge: " << message;

        ++iEdges;
    }

    if (client == NULL){
        foreach(QTcpSocket *otherClient, clients){
            otherClient->write("GRAPH_END\n");
        }
    } else {
        client -> write("GRAPH_END\n");
    }

    //qDebug() << "Sending took" << t.elapsed() << "ms";

}

void Server::sendLayout(QTcpSocket *client){

    if (!this -> isListening() || (client == NULL && clients.size() == 0)) {
        return;
    }

    QString message;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

    if (currentGraph == NULL) {
        return;
    }

    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator iNodes =  nodes->constBegin();


    /*QTime t;
    t.start();*/

    if (client == NULL){
        foreach(QTcpSocket *otherClient, clients){
            otherClient->write("LAYOUT_START\n");
        }
    } else {
        client -> write("LAYOUT_START\n");
    }

    while(iNodes != nodes->constEnd()) {

        message = "id:" + QString::number(iNodes.value()->getId());
        message += ";x:" + QString::number(iNodes.value()->getCurrentPosition().x()/graphScale);
        message += ";y:" + QString::number(iNodes.value()->getCurrentPosition().y()/graphScale);
        message += ";z:" + QString::number(iNodes.value()->getCurrentPosition().z()/graphScale);

        if (client == NULL){
            foreach(QTcpSocket *otherClient, clients){
                otherClient->write(("/layData:"+message+"\n").toUtf8());
            }
        } else {
            client -> write(("/layData:"+message+"\n").toUtf8());
        }

        ++iNodes;
    }

    if (client == NULL){
        foreach(QTcpSocket *otherClient, clients){
            otherClient->write("LAYOUT_END\n");
        }
    } else {
        client -> write("LAYOUT_END\n");
    }

    //qDebug() << "Sending layout took" << t.elapsed() << "ms";

}

void Server::setLayoutThread(Layout::LayoutThread *layoutThread){
    thread = layoutThread;
}

void Server::stopServer(){

    foreach(QTcpSocket *otherClient, clients){
        otherClient->write("SERVER_STOP\n");
    }

    clients.clear();
    users.clear();

    this->close();

}

void Server::sendMoveNodes() {
    QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selected_nodes.constBegin();
    QString message;

    while (i != selected_nodes.constEnd())
    {

        message = "id:" + QString::number((*i)->getId());
        message += ";x:" + QString::number((*i)->getCurrentPosition().x()/graphScale);
        message += ";y:" + QString::number((*i)->getCurrentPosition().y()/graphScale);
        message += ";z:" + QString::number((*i)->getCurrentPosition().z()/graphScale);
        ++i;

        foreach(QTcpSocket *otherClient, clients){
            otherClient->write(QString("/moveNode:" + message + "\n").toUtf8());
        }
    }

    selected_nodes.clear();
}


void Server::sendMyView(osg::Vec3d center, osg::Quat rotation, QTcpSocket * client) {

    QString message = "/view:";
    message += "center:" + QString::number(center.x()) + "," + QString::number(center.y()) + "," + QString::number(center.z()) + ";";
    message += "rotation:" + QString::number(rotation.x()) + "," + QString::number(rotation.y()) + "," + QString::number(rotation.z()) + "," + QString::number(rotation.w()) +  ";id:0\n";

    if (client == NULL) {
        foreach(QTcpSocket *otherClient, clients){
            otherClient->write(message.toUtf8());
        }
    } else {
        client->write(message.toUtf8());
    }

}

void Server::sendMyView(QTcpSocket *client) {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    this->sendMyView(cameraManipulator->getCenter(), cameraManipulator->getRotation(), client);
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

osg::PositionAttitudeTransform * Server::generateAvatar() {
    osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("avatar.osg");
    if (!modelNode) {
        qDebug() << "could not find model";
        return NULL;
    }

    osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
    PAtransform->addChild(modelNode);

    QLinkedList<osg::ref_ptr<osg::Node> > * nodes = coreGraph->getCustomNodeList();

    nodes->append(PAtransform);

    //PAtransform->setScale(osg::Vec3d(10,10,10));

    return PAtransform;
}

void Server::removeAvatar(QTcpSocket *client) {

    osg::PositionAttitudeTransform *pat = avatars.take(client);
    if (pat != NULL) {
        pat->removeChild(0,1);
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
    removeAvatar(user_to_spy);

    QString message = ("/spying:"+QString::number(id)+";spy:0\n");

    foreach(QTcpSocket *client, clients) {
        client->write(message.toUtf8());
    }
}

void Server::unSpyUser() {

    addAvatar(user_to_spy,generateAvatar());
    QString message = "/unspy:"+QString::number(getUserId(user_to_spy))+";spy:0\n";

    foreach(QTcpSocket *client, clients) {
        client->write(message.toUtf8());
    }

    user_to_spy = NULL;
}

void Server::setMyView(osg::Vec3d center, osg::Quat rotation) {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    cameraManipulator->setCenter(center);
    cameraManipulator->setRotation(rotation);
}
