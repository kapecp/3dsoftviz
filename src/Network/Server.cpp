/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Server.h"

#include <QTcpSocket>
#include <QRegExp>

using namespace Network;

Server::Server(QObject *parent) : QTcpServer(parent)
{
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

            foreach(QTcpSocket *otherClient, clients)
                otherClient->write(QString(user + ":" + message + "\n").toUtf8());
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
