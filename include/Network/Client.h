/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#ifndef __3DVisualClient_H__
#define __3DVisualClient_H__

#include <QTcpSocket>
#include <QStringList>

#include "Data/Graph.h"
#include "Viewer/CoreGraph.h"
#include "Layout/LayoutThread.h"

#include "osg/PositionAttitudeTransform"

namespace Network{

class Client : public QObject
{
    Q_OBJECT

    public:

        Client(QObject *parent=0);

        static Client* getInstance();

        void ServerConnect(QString nick, QString address);

        void send_message(QString message);

        void setLayoutThread(Layout::LayoutThread * layoutThread);
        void setCoreGraph(Vwr::CoreGraph * cg) { coreGraph = cg; }

        bool isConnected();

        void disconnect();

        void setNodesExcludedFromUpdate(QLinkedList<osg::ref_ptr<Data::Node> > nodes) { selected_nodes = nodes; }
        void sendMovedNodesPosition();

        void sendMyView(osg::Vec3d center, osg::Quat rotation);

    private slots:
        // This is a function we'll connect to a socket's readyRead()
        // signal, which tells us there's something to be read from the server.
        void readyRead();

        // This function gets called when the socket tells us it's connected.
        void connected();

        void error();

        void disconnected();

    private:
        static Client *instance;

        QMap<int,QString> userList;
        QMap<int,osg::PositionAttitudeTransform*> avatarList;

        QString clientNick;

        std::map<qlonglong, osg::ref_ptr<Data::Node> > nodes;
        QLinkedList<osg::ref_ptr<Data::Node> > selected_nodes;
        Data::Type *nodeType;
        Data::Type *edgeType;

        Layout::LayoutThread * thread;
        Vwr::CoreGraph * coreGraph;
        QObject * cw;

        // This is the socket that will let us communitate with the server.
        QTcpSocket *socket;

    };

}

#endif
