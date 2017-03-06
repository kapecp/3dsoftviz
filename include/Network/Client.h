/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#ifndef Client_H
#define Client_H

#include <QByteArray>
#include <QDebug>
#include <QLinkedList>
#include <QListWidgetItem>
#include <QTcpSocket>

// TODO: typedef inside class
#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"

#include "osg/PositionAttitudeTransform"

namespace Data {
class Graph;
}

namespace Vwr {
class CoreGraph;
}

namespace Layout {
class LayoutThread;
}

namespace Network {

class ExecutorFactory;


class Client : public QObject
{
	Q_OBJECT

public:

	explicit Client( QObject* parent=0 );

	static Client* getInstance();

	void ServerConnect( QString nick, QString address );

	void send_message( QString message );
	void requestGraph();

	void setLayoutThread( Layout::LayoutThread* layoutThread );
	void setCoreGraph( Vwr::CoreGraph* cg )
	{
		coreGraph = cg;
	}

	bool isConnected();

	void disconnect();

	void setNodesExcludedFromUpdate( QLinkedList<osg::ref_ptr<Data::Node> > nodes )
	{
		selected_nodes = nodes;
	}
	void sendMovedNodesPosition();
	void updateUserList();

	void addClient( int id, QString nick );
	void removeAvatar( int id );
	void showClientAvatar( int id );

	void spyUser( int user );
	int userToSpy()
	{
		return user_to_spy;
	}
	bool isSpying()
	{
		return user_to_spy > -1;
	}
	void unSpyUser();

	void centerUser( int id_user );
	void unCenterUser();
	int getCenterUser()
	{
		return user_to_center;
	}
	bool isCenteringUser()
	{
		return user_to_center > -1;
	}

	void sendMyView( osg::Vec3d center, osg::Quat rotation, float distance );
	void sendMyView();
	void setMyView( osg::Vec3d center, osg::Quat rotation, double distance );
	void lookAt( osg::Vec3d coord );

	void sendNewNode( QString name, osg::Vec3f position );

	void sendNewEdge( QString name, qlonglong id_from, qlonglong id_to, bool oriented = false );
	void sendRemoveNode( qlonglong id );
	void sendRemoveEdge( qlonglong id );
	void sendNodeColor( qlonglong id, float r, float g, float b, float alpha );
	void sendEdgeColor( int id, float r, float g, float b, float alpha );
	void sendNodeLabel( qlonglong id, QString label );
	void sendFixNodeState( qlonglong id, bool state );
	void sendMergeNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, osg::Vec3f position );
	void sendSeparateNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes );
	void sendAddMetaNode( QString name, QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, QString edgeName, osg::Vec3f position );
	void sendSetRestriction( quint8 type, QString name_node1, osg::Vec3 position_node1, QString name_node2, osg::Vec3 position_node2, QLinkedList<osg::ref_ptr<Data::Node> >* nodes, QString name_node3 = "", osg::Vec3* position_node3 = NULL );
	void sendSetRestriction( quint8 type, QLinkedList<osg::ref_ptr<Data::Node> >* nodes,Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType* restrictionNodes );
	void sendUnSetRestriction( QLinkedList<osg::ref_ptr<Data::Node> >* nodes );
	void sendAttractAttention( bool attention );

	void setAttention( int user );
	void unSetAttention( int user );

	void setMyId( int id )
	{
		my_id = id;
	}
	int getMyId()
	{
		return my_id;
	}
	void ignoreLayout( bool ignore )
	{
		layoutIgnore = ignore;
	}
	bool ignoreLayout()
	{
		return layoutIgnore;
	}

	void setAvatarScale( int scale );

	QObject* getCoreWindowReference()
	{
		return cw;
	}

	//todo: encapsulate!
	QMap<int,QString> userList;
	QMap<int,osg::PositionAttitudeTransform*> avatarList;
	Vwr::CoreGraph* coreGraph;
	Layout::LayoutThread* thread;
	Data::Graph* currentGraph;
	Data::Type* nodeType;
	Data::Type* edgeType;
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
	static Client* instance;

	QString clientNick;
	int user_to_spy;
	int user_to_center;
	int my_id;
	bool layoutIgnore;

	osg::Vec3d original_center;
	osg::Quat original_rotation;
	double original_distance;

	ExecutorFactory* executorFactory;

	QObject* cw;

	// This is the socket that will let us communitate with the server.
	QTcpSocket* socket;

	void addAvatar( int id, QString nick );

	void sendColor( quint8 instruction, qlonglong id, float r, float g, float b, float alpha );
	QListWidgetItem* getItemById( int id );

	quint16 blockSize;
	int avatarScale;

};

}

#endif
