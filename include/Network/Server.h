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

#include "osg/PositionAttitudeTransform"

#include "Layout/LayoutThread.h"
#include "Viewer/CoreGraph.h"

namespace Network {

class Server : public QTcpServer {

    Q_OBJECT

    public:
        Server(QObject *parent=0);

        static Server* getInstance();

        void setLayoutThread(Layout::LayoutThread * layoutThread);
        void setCoreGraph(Vwr::CoreGraph * cg) { coreGraph = cg; }

        void sendGraph(QTcpSocket *client = NULL);
        void sendLayout(QTcpSocket *client = NULL);
        void sendMoveNodes();

        void stopServer();

        void setSelectedNodes(QLinkedList<osg::ref_ptr<Data::Node> > nodes) { selected_nodes = nodes; }

        void sendMyView(osg::Vec3d center, osg::Quat rotation);

    private slots:
        void readyRead();
        void disconnected();
        void sendUserList();

    protected:
        void incomingConnection(int socketfd);

    private:
        static Server *instance;

        QLinkedList<osg::ref_ptr<Data::Node> > selected_nodes;
        QLinkedList<osg::ref_ptr<Data::Node> > moving_nodes;

        QSet<QTcpSocket*> clients;
        float graphScale;
        QMap<QTcpSocket*,QString> users;
        QMap<QTcpSocket*,int> usersID;
        QMap<QTcpSocket*,osg::PositionAttitudeTransform *> avatars;
        Layout::LayoutThread * thread;
        Vwr::CoreGraph * coreGraph;
    };
}

#endif
