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
#include "Network/ExecutorFactory.h"

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
        void updateUserList();

        void addClient(int id, QString nick);
        void hideClientAvatar(int id);
        void showClientAvatar(int id);

        void spyUser(int user);
        int userToSpy() { return user_to_spy; }
        bool isSpying() { return user_to_spy > -1; }
        void unSpyUser();

        void centerUser(int id_user);
        void unCenterUser() { user_to_center = -1; }
        int getCenterUser() { return user_to_center; }
        bool isCenteringUser() { return user_to_center > -1; }

        void sendMyView(osg::Vec3d center, osg::Quat rotation);
        void sendMyView();
        void setMyView(osg::Vec3d center, osg::Quat rotation);
        void lookAt(osg::Vec3d coord);


        void setMyId(int id) { my_id = id; }
        int getMyId() { return my_id; }

        //todo: encapsulate!
        QMap<int,QString> userList;
        QMap<int,osg::PositionAttitudeTransform*> avatarList;
        Vwr::CoreGraph * coreGraph;
        Layout::LayoutThread * thread;
        Data::Graph * currentGraph;
        Data::Type *nodeType;
        Data::Type *edgeType;
        QLinkedList<osg::ref_ptr<Data::Node> > selected_nodes;

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

        QString clientNick;
        int user_to_spy;
        int user_to_center;
        int my_id;

        osg::Vec3d original_center;
        osg::Quat original_rotation;

        ExecutorFactory *executorFactory;

        QObject * cw;

        // This is the socket that will let us communitate with the server.
        QTcpSocket *socket;

        void addAvatar(int id);

        quint16 blockSize;

    };

}

#endif
