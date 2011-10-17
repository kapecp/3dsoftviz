/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#ifndef __3DVisualServer_H__
#define __3DVisualServer_H__

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>

#include "Layout/LayoutThread.h"

namespace Network {

class Server : public QTcpServer {

    Q_OBJECT

    public:
        Server(QObject *parent=0);
        void setLayoutThread(Layout::LayoutThread * layoutThread);

    private slots:
        void readyRead();
        void disconnected();
        void sendUserList();

    protected:
        void incomingConnection(int socketfd);

    private:
        QSet<QTcpSocket*> clients;
        QMap<QTcpSocket*,QString> users;
        Layout::LayoutThread * thread;
        void sendGraph(QTcpSocket *client);
    };
}

#endif
