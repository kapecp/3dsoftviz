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
    Util::ApplicationConfig *conf = Util::ApplicationConfig::get();
    graphScale = conf->getValue("Viewer.Display.NodeDistanceScale").toFloat();
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
        QRegExp moveNodeRegexp("^/moveNode:id:([0-9]+);x:([0-9-\\.]+);y:([0-9-\\.]+);z:([0-9-\\.]+)$");
        QRegExp viewRegexp("^/view:center:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);rotation:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+)$");
        qDebug() << "Read line:" << line;

        QRegExp meRegex("^/me:(.*)$");

        if(meRegex.indexIn(line) != -1)
        {
            QString user = meRegex.cap(1);
            users[senderClient] = user;
            int newID = 1;
            if (usersID.count() > 0) {
                QList<int> IDs = usersID.values();
                newID = (*(std::max_element(IDs.begin(),IDs.end())))+1;
            }
            usersID[senderClient] = newID;

            osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("avatar.osg");
            if (!modelNode) {
                qDebug() << "could not find model";
                return;
            }

            osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
            PAtransform->addChild(modelNode);

            QLinkedList<osg::ref_ptr<osg::Node> > * nodes = coreGraph->getCustomNodeList();

            nodes->append(PAtransform);

            PAtransform->setScale(osg::Vec3d(10,10,10));
            avatars.insert(senderClient,PAtransform);

            senderClient->write("WELCOME\n");
            sendUserList();
        }
        else if (moveNodeRegexp.indexIn(line) != -1) {

            int id = moveNodeRegexp.cap(1).toInt();

            float x = moveNodeRegexp.cap(2).toFloat()/graphScale;
            float y = moveNodeRegexp.cap(3).toFloat()/graphScale;
            float z = moveNodeRegexp.cap(4).toFloat()/graphScale;

            QString message = "/moveNode:id:"+ QString::number(id) + ";x:" + QString::number(x) + ";y:" + QString::number(y) + ";z:" + QString::number(z) + "\n";
            foreach(QTcpSocket *otherClient, clients) {
                otherClient->write(message.toUtf8());
            }

            Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
            QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
            //qDebug() << "Moving" << id << "to" << x << y << z;
            Data::Node *node = (*((*nodes).find(id)));

            moving_nodes.append(node);

            node -> setUsingInterpolation(false);
            node -> setFixed(true);
            node -> setTargetPosition(osg::Vec3(x,y,z));
            thread->play();
        }
        else if (viewRegexp.indexIn(line) != -1) {
            foreach(QTcpSocket *client, clients) { // append sender ID and resend to all other clients except sender
                if (client == senderClient) continue;
                client->write((line+";id:"+QString::number(usersID[senderClient])+"\n").toUtf8());
            }

            osg::Vec3d center = osg::Vec3d(viewRegexp.cap(1).toFloat()-5,viewRegexp.cap(2).toFloat(),viewRegexp.cap(3).toFloat());
            osg::Quat rotation = osg::Quat(viewRegexp.cap(4).toFloat(),viewRegexp.cap(5).toFloat(),viewRegexp.cap(6).toFloat(),viewRegexp.cap(7).toFloat());

            osg::PositionAttitudeTransform * PAtransform = avatars[senderClient];
            if (PAtransform != NULL) {
                PAtransform->setAttitude(rotation);
                PAtransform->setPosition(center);
            }
        }
        else if(users.contains(senderClient))
        {
            QString message = line;
            QString user = users[senderClient];
            qDebug() << "User:" << user;
            qDebug() << "Message:" << message;

            if (message == "GET_GRAPH") {
                sendGraph(senderClient);
            } else if (message == "GET_LAYOUT") {
                sendLayout(senderClient);
            } else {
                foreach(QTcpSocket *otherClient, clients)
                    otherClient->write(QString(user + ":" + message + "\n").toUtf8());
            }
        }
        else
        {
            qWarning() << "Got bad message from client:" << senderClient->peerAddress().toString() << line;
        }
    }
}

void Server::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clients.remove(client);
    users.remove(client);

    sendUserList();
}

void Server::sendUserList()
{
    QStringList userList;

    QMap<QTcpSocket*,QString>::const_iterator i;

    foreach(QTcpSocket *client, clients){
        userList.clear();
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


    /*bool isRunning = thread->isRunning();

    if (isRunning) {
        thread->pause();
        coreGraph->setNodesFreezed(true);
    }*/
    QTime t;
    t.start();

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

    qDebug() << "Sending took" << t.elapsed() << "ms";

    /*if (isRunning) {
        thread->play();
        coreGraph->setNodesFreezed(false);
    }*/

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


    QTime t;
    t.start();

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

        //qDebug() << "[SERVER] Sending node: " << message;

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


void Server::sendMyView(osg::Vec3d center, osg::Quat rotation) {

    QString message = "/view:";
    message += "center:" + QString::number(center.x()) + "," + QString::number(center.y()) + "," + QString::number(center.z()) + ";";
    message += "rotation:" + QString::number(rotation.x()) + "," + QString::number(rotation.y()) + "," + QString::number(rotation.z()) + "," + QString::number(rotation.w()) +  ";id:0\n";

    foreach(QTcpSocket *otherClient, clients){
        otherClient->write(message.toUtf8());
    }

}
