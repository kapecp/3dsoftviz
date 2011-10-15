/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"
#include <QRegExp>
#include "Data/Graph.h"
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

        QRegExp messageRegex("^([^:]+):(.*)$");

        QRegExp usersRegex("^/clients:(.*)$");

        if(usersRegex.indexIn(line) != -1)
        {
            QStringList users = usersRegex.cap(1).split(",");
            qDebug() << "Clients:";
            foreach(QString user, users)
                qDebug() << user;
        }
        else if(messageRegex.indexIn(line) != -1)
        {
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            qDebug() << user + ": " + message;


            /*Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
            Data::Type *edgeType = NULL;
            Data::Type *nodeType = NULL;*/

            QRegExp dataRegexp("^id:([0-9]+);x:([0-9-\\.]+);y:([0-9-\\.]+);z:([0-9-\\.]+)$");
            if (dataRegexp.indexIn(message) != -1){
                int id = dataRegexp.cap(1).toInt();
                float x = dataRegexp.cap(2).toFloat();
                float y = dataRegexp.cap(3).toFloat();
                float z = dataRegexp.cap(4).toFloat();

                qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

                Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

                if (currentGraph == NULL) {
                    currentGraph= Manager::GraphManager::getInstance()->createNewGraph("NewGraph");
                }

                osg::Vec3 position(x,y,z);
                currentGraph->addNode("newNode", currentGraph->getNodeMetaType(), position);

                /*if (currentGraph != NULL)
                {

                }
                else
                {

                    osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);
                    Data::MetaType* type = currentGraph->addMetaType(Data::GraphLayout::META_NODE_TYPE);
                    osg::ref_ptr<Data::Node> node1 = currentGraph->addNode("newNode", type);
                }*/
            }

        }
    }
}

void Client::connected()
{
    // And send our username to the chat server.
    socket->write(QString("/me:" + clientNick + "\n").toUtf8());
}
