/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"
#include <QRegExp>
#include "Manager/Manager.h"

using namespace Network;

Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
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
        qDebug() << "Client got line: " << line;

        QRegExp messageRegex("^([^:]+):(.*)$");

        QRegExp usersRegex("^/clients:(.*)$");

        QRegExp nodeRegexp("^/nodeData:id:([0-9]+);x:([0-9-\\.]+);y:([0-9-\\.]+);z:([0-9-\\.]+)$");

        QRegExp edgeRegexp("^/edgeData:id:([0-9]+);from:([0-9]+);to:([0-9]+)$");

        Data::MetaType* type = NULL;
        //QMap<qlonglong, osg::ref_ptr<Data::Node> > *nodes = NULL;

        Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

        if(usersRegex.indexIn(line) != -1)
        {
            QStringList users = usersRegex.cap(1).split(",");
            qDebug() << "Clients:";
            foreach(QString user, users)
                qDebug() << user;
        } else if (nodeRegexp.indexIn(line) != -1){
            int id = nodeRegexp.cap(1).toInt();
            float x = nodeRegexp.cap(2).toFloat();
            float y = nodeRegexp.cap(3).toFloat();
            float z = nodeRegexp.cap(4).toFloat();

            qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";


            if (currentGraph == NULL) {
                currentGraph= Manager::GraphManager::getInstance()->createNewGraph("NewGraph");
            }

            type = currentGraph->addMetaType(Data::GraphLayout::META_NODE_TYPE);
            osg::Vec3 position(x,y,z);
            osg::ref_ptr<Data::Node> node = currentGraph->addNode(id,"newNode", type, position);
            nodes[id] = node;

        } else if (edgeRegexp.indexIn(line) != -1) {
            int id = edgeRegexp.cap(1).toInt();
            int from = edgeRegexp.cap(2).toInt();
            int to = edgeRegexp.cap(3).toInt();

            Data::Type* type = currentGraph->addType(Data::GraphLayout::META_EDGE_TYPE);

            qDebug()<< "[NEW EDGE] id: " << id << " from: " << from << ", to:" << to;

            currentGraph->addEdge(id,"NewEdge",nodes[from],nodes[to],type,false);
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
