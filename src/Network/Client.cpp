/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"
#include "Importer/GraphOperations.h"
#include <QRegExp>
#include "Manager/Manager.h"

using namespace Network;

Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));

    edgeType = NULL;
    nodeType = NULL;
}

void Client::ServerConnect(QString nick)
{
    clientNick = nick;
    socket->connectToHost("localhost", 4200);
}

void Client::send_message(QString message)
{
    if(!message.isEmpty())
    {
        socket->write(QString(message + "\n").toUtf8());
    }
}

// This function gets called whenever the chat server has sent us some text:
void Client::readyRead()
{
    // We'll loop over every (complete) line of text that the server has sent us:
    while(socket->canReadLine())
    {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        //qDebug() << "Client got line: " << line;

        QRegExp messageRegex("^([^:]+):(.*)$");

        QRegExp usersRegex("^/clients:(.*)$");

        QRegExp nodeRegexp("^/nodeData:id:([0-9]+);x:([0-9-\\.]+);y:([0-9-\\.]+);z:([0-9-\\.]+)$");

        QRegExp edgeRegexp("^/edgeData:id:([0-9]+);from:([0-9]+);to:([0-9]+);or:([01])$");

        Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

        if(usersRegex.indexIn(line) != -1)
        {
            QStringList users = usersRegex.cap(1).split(",");
            qDebug() << "Clients:";
            foreach(QString user, users)
                qDebug() << user;
        } else if (line == "GRAPH_START") {
            currentGraph= Manager::GraphManager::getInstance()->createNewGraph("NewGraph");
            Importer::GraphOperations * operations = new Importer::GraphOperations(*currentGraph);
            operations->addDefaultTypes(edgeType, nodeType);

            thread->pause();
            coreGraph->setNodesFreezed(true);
            currentGraph->setFrozen(true);
        } else if (line == "GRAPH_END") {
            thread->play();
            coreGraph->setNodesFreezed(false);
            currentGraph->setFrozen(false);
        } else if (nodeRegexp.indexIn(line) != -1){
            int id = nodeRegexp.cap(1).toInt();
            float x = nodeRegexp.cap(2).toFloat()/2;
            float y = nodeRegexp.cap(3).toFloat()/2;
            float z = nodeRegexp.cap(4).toFloat()/2;

            qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

            osg::Vec3 position(x,y,z);
            osg::ref_ptr<Data::Node> node = currentGraph->addNode(id,"newNode", nodeType, position);
            nodes[id] = node;

        } else if (edgeRegexp.indexIn(line) != -1) {
            int id = edgeRegexp.cap(1).toInt();
            int from = edgeRegexp.cap(2).toInt();
            int to = edgeRegexp.cap(3).toInt();
            bool oriented = edgeRegexp.cap(4).toInt();

            qDebug()<< "[NEW EDGE] id: " << id << " from: " << from << ", to:" << to;

            currentGraph->addEdge(id,"NewEdge",nodes[from],nodes[to],edgeType,oriented);
        }
        else if(messageRegex.indexIn(line) != -1)
        {
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            qDebug() << user + ": " + message;

        }
    }
}

void Client::connected()
{
    // And send our username to the chat server.
    socket->write(QString("/me:" + clientNick + "\n").toUtf8());
}

void Client::setLayoutThread(Layout::LayoutThread *layoutThread){
    thread = layoutThread;
}
