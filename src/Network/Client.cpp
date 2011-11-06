/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"
#include "Importer/GraphOperations.h"
#include <QRegExp>
#include "Manager/Manager.h"
#include "QOSG/CoreWindow.h"

using namespace Network;

Client * Client::instance;

Client::Client(QObject *parent) : QObject(parent) {

    instance = this;

    socket = new QTcpSocket(this);

    cw = parent;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
    //connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    edgeType = NULL;
    nodeType = NULL;

}

Client* Client::getInstance() {
    if (instance == NULL) {
        return new Client();
    } else {
        return instance;
    }
}

void Client::ServerConnect(QString nick, QString address) {
    clientNick = nick;
    socket->connectToHost(address, 4200);
}

void Client::send_message(QString message) {
    if(!message.isEmpty())
    {
        socket->write(QString(message + "\n").toUtf8());
    }
}

// This function gets called whenever the chat server has sent us some text:
void Client::readyRead() {
    // We'll loop over every (complete) line of text that the server has sent us:
    while(socket->canReadLine())
    {
        QTime t;
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        //qDebug() << "Client got line: " << line;

        QRegExp messageRegex("^([^:]+):(.*)$");

        QRegExp usersRegex("^/clients:(.*)$");

        QRegExp nodeRegexp("^/nodeData:id:([0-9]+);x:([0-9-\\.]+);y:([0-9-\\.]+);z:([0-9-\\.]+)$");

        QRegExp layRegexp("^/layData:id:([0-9]+);x:([0-9-\\.]+);y:([0-9-\\.]+);z:([0-9-\\.]+)$");

        QRegExp edgeRegexp("^/edgeData:id:([0-9]+);from:([0-9]+);to:([0-9]+);or:([01])$");

        Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

        if(usersRegex.indexIn(line) != -1)
        {
            QStringList users = usersRegex.cap(1).split(",");
            qDebug() << "Clients:";
            foreach(QString user, users)
                qDebug() << user;
        } else if (line == "GRAPH_START") {
            t.start();
            currentGraph= Manager::GraphManager::getInstance()->createNewGraph("NewGraph");
            Importer::GraphOperations * operations = new Importer::GraphOperations(*currentGraph);
            operations->addDefaultTypes(edgeType, nodeType);

            thread->pause();
            coreGraph->setNodesFreezed(true);
            currentGraph->setFrozen(true);
        } else if (line == "GRAPH_END") {
            qDebug() << "Building took" << QString::number(t.elapsed()) << "ms";
            thread->play();
            coreGraph->setNodesFreezed(false);
            currentGraph->setFrozen(false);
        } else if (nodeRegexp.indexIn(line) != -1){
            int id = nodeRegexp.cap(1).toInt();

            float x = nodeRegexp.cap(2).toFloat();
            float y = nodeRegexp.cap(3).toFloat();
            float z = nodeRegexp.cap(4).toFloat();

            //qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

            osg::Vec3 position(x,y,z);
            osg::ref_ptr<Data::Node> node = currentGraph->addNode(id,"newNode", nodeType, position);
            nodes[id] = node;

        } else if (edgeRegexp.indexIn(line) != -1) {
            int id = edgeRegexp.cap(1).toInt();
            int from = edgeRegexp.cap(2).toInt();
            int to = edgeRegexp.cap(3).toInt();
            bool oriented = edgeRegexp.cap(4).toInt();

            //qDebug()<< "[NEW EDGE] id: " << id << " from: " << from << ", to:" << to;

            currentGraph->addEdge(id,"NewEdge",nodes[from],nodes[to],edgeType,oriented);
        } else if (layRegexp.indexIn(line) != -1) {
            int id = layRegexp.cap(1).toInt();

            float x = layRegexp.cap(2).toFloat();
            float y = layRegexp.cap(3).toFloat();
            float z = layRegexp.cap(4).toFloat();

            thread->pause();

            if (!excluded_nodes.contains(nodes[id])){
                nodes[id]->setTargetPosition(osg::Vec3(x,y,z));
            }

            //qDebug()<< "[NEW NODE POS] id: " << id << " [" << x << "," << y << "," << z << "]";

        } else if(messageRegex.indexIn(line) != -1) {
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            qDebug() << user + ": " + message;

        } else if (line == "SERVER_STOP") {
            this->disconnect();
        }
    }
}

void Client::connected()
{
    socket->write(QString("/me:" + clientNick + "\n").toUtf8());
    ((QOSG::CoreWindow *) cw) -> le_client_name -> setEnabled(false);
    ((QOSG::CoreWindow *) cw) -> le_server_addr -> setEnabled(false);
    ((QOSG::CoreWindow *) cw) -> b_start_client -> setEnabled(true);
    ((QOSG::CoreWindow *) cw) -> b_start_client -> setText("Disconnect");
}

void Client::error(){
    this->disconnect();
}

void Client::disconnected(){
    this->disconnect();
}

void Client::disconnect() {
    if (socket -> isOpen()){
        socket -> disconnectFromHost();
        socket -> close();
    }
    ((QOSG::CoreWindow *) cw) -> le_client_name -> setEnabled(true);
    ((QOSG::CoreWindow *) cw) -> le_server_addr -> setEnabled(true);
    ((QOSG::CoreWindow *) cw) -> b_start_client -> setEnabled(true);
    ((QOSG::CoreWindow *) cw) -> b_start_client -> setText("Connect to session");
    ((QOSG::CoreWindow *) cw) -> b_start_server -> setEnabled(true);
}

void Client::setLayoutThread(Layout::LayoutThread *layoutThread){
    thread = layoutThread;
}

bool Client::isConnected() {
    return socket -> state() == QAbstractSocket::ConnectedState;
}

void Client::sendExcludedNodesPosition() {
    QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = excluded_nodes.constBegin();
    QString message;

    while (i != excluded_nodes.constEnd())
    {

        message = "id:" + QString::number((*i)->getId());
        message += ";x:" + QString::number((*i)->getCurrentPosition().x());
        message += ";y:" + QString::number((*i)->getCurrentPosition().y());
        message += ";z:" + QString::number((*i)->getCurrentPosition().z());
        ++i;

        socket->write(QString("/moveNode:" + message + "\n").toUtf8());
    }
}

void Client::clearNodesExcludedFromUpdate() {
    excluded_nodes.clear();
}
