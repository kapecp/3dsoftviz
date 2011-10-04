#include "Network/Client.h"
#include <QRegExp>

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
        }
    }
}

void Client::connected()
{
    // And send our username to the chat server.
    socket->write(QString("/me:" + clientNick + "\n").toUtf8());
}
