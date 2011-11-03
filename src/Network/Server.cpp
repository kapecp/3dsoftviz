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
    QTcpSocket *client = (QTcpSocket*)sender();
    while(client->canReadLine())
    {
        QString line = QString::fromUtf8(client->readLine()).trimmed();
        qDebug() << "Read line:" << line;

        QRegExp meRegex("^/me:(.*)$");

        if(meRegex.indexIn(line) != -1)
        {
            client -> write("WELCOME\n");
            QString user = meRegex.cap(1);
            users[client] = user;
            foreach(QTcpSocket *client, clients)
                client->write(QString("Server:" + user + " has joined.\n").toUtf8());
            sendUserList();
        }
        else if(users.contains(client))
        {
            QString message = line;
            QString user = users[client];
            qDebug() << "User:" << user;
            qDebug() << "Message:" << message;

            if (message == "GET_GRAPH") {
                sendGraph(client);
            } else if (message == "GET_LAYOUT") {
                sendLayout(client);
            } else {
                foreach(QTcpSocket *otherClient, clients)
                    otherClient->write(QString(user + ":" + message + "\n").toUtf8());
            }
        }
        else
        {
            qWarning() << "Got bad message from client:" << client->peerAddress().toString() << line;
        }
    }
}

void Server::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clients.remove(client);

    QString user = users[client];
    users.remove(client);

    sendUserList();
    foreach(QTcpSocket *client, clients)
        client->write(QString("Server:" + user + " has left.\n").toUtf8());
}

void Server::sendUserList()
{
    QStringList userList;
    foreach(QString user, users.values())
        userList << user;

    foreach(QTcpSocket *client, clients)
        client->write(QString("/clients:" + userList.join(",") + "\n").toUtf8());
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
