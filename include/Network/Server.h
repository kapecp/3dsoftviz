/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#ifndef Server_H
#define Server_H

#include <QByteArray>
#include <QLinkedList>
#include <QListWidgetItem>
#include <QMap>
#include <QSet>
#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>

#include "osg/PositionAttitudeTransform"

#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"

namespace Layout {
class LayoutThread;
}

namespace Vwr {
class CoreGraph;
}


namespace Network {

class ExecutorFactory;

class Server : public QTcpServer
{

	Q_OBJECT

public:
	explicit Server( QObject* parent=0 );

	static Server* getInstance();

	void sendGraph( QTcpSocket* client = NULL );
	void sendLayout( QTcpSocket* client = NULL );
	void sendNewNode( osg::ref_ptr<Data::Node> node, QTcpSocket* client = NULL );
	void sendNewEdge( osg::ref_ptr<Data::Edge> edge, QTcpSocket* client = NULL );

	void sendRemoveNode( qlonglong id, QTcpSocket* client = NULL );
	void sendRemoveEdge( qlonglong id, QTcpSocket* client = NULL );
	void sendMoveNodes();
	void sendNodeColor( qlonglong id, float r, float g, float b, float alpha, QTcpSocket* client = NULL );
	void sendEdgeColor( qlonglong id, float r, float g, float b, float alpha, QTcpSocket* client = NULL );
	void sendNodeLabel( qlonglong id, QString label, QTcpSocket* client = NULL );
	void sendFixNodeState( qlonglong id, bool state, QTcpSocket* client = NULL );
	void sendMergeNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, osg::Vec3f position, qlonglong mergeNodeId, QTcpSocket* client = NULL );
	void sendSeparateNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, QTcpSocket* client = NULL );
	void sendAddMetaNode( osg::ref_ptr<Data::Node> metaNode, QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, QString edgeName, osg::Vec3f position, QTcpSocket* client = NULL );
	void sendSetRestriction( quint8 type, osg::ref_ptr<Data::Node> node1, osg::Vec3 position_node1,  osg::ref_ptr<Data::Node> node2, osg::Vec3 position_node2, QLinkedList<osg::ref_ptr<Data::Node> >* nodes, osg::ref_ptr<Data::Node> node3 = NULL, osg::Vec3* position_node3 = NULL, QTcpSocket* client = NULL );
	void sendSetRestriction( quint8 type, QLinkedList<osg::ref_ptr<Data::Node> >* nodes, Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType* restrictionNodes, QTcpSocket* client=NULL );
	void sendUnSetRestriction( QLinkedList<osg::ref_ptr<Data::Node> >* nodes, QTcpSocket* client = NULL );
	void sendAttractAttention( bool attention, int idUser = 0, QTcpSocket* client = NULL );

	void setAttention( int user );
	void unSetAttention( int user );

	void setAvatarScale( int scale );

	void stopServer();

	void setSelectedNodes( QLinkedList<osg::ref_ptr<Data::Node> > nodes )
	{
		selected_nodes = nodes;
	}

	void sendMyView( osg::Vec3d center, osg::Quat rotation, double distance, QTcpSocket* client = NULL );
	void sendMyView( QTcpSocket* client );
	void sendMyView();

	void sendUserList();
	void updateUserList();

	void removeAvatar( QTcpSocket* client );
	void spyUser( int id );
	bool isSpying()
	{
		return user_to_spy != NULL;
	}
	void unSpyUser();

	void centerUser( int id_user );
	void unCenterUser();
	QTcpSocket* getCenterUser()
	{
		return user_to_center;
	}
	bool isCenteringUser()
	{
		return user_to_center != NULL;
	}

	void setMyView( osg::Vec3d center, osg::Quat rotation, double distance );
	void lookAt( osg::Vec3d coord );

	//some getters
	float getGraphScale()
	{
		return graphScale;
	}
	QSet<QTcpSocket*> getClients()
	{
		return clients;
	}
	Layout::LayoutThread* getLayoutThread()
	{
		return thread;
	}
	int getUserId( QTcpSocket* Client )
	{
		return usersID[Client];
	}
	osg::PositionAttitudeTransform* getAvatarTransform( QTcpSocket* client )
	{
		return avatars[client];
	}
	int getUserCount()
	{
		return usersID.count();
	}
	int getMaxUserId();
	Vwr::CoreGraph* getCoreGraph()
	{
		return coreGraph;
	}
	QTcpSocket* userToSpy()
	{
		return user_to_spy;
	}
	QString getUserName( QTcpSocket* user )
	{
		return users[user];
	}

	//some setters
	void appendMovingNode( osg::ref_ptr<Data::Node> node );
	void addUser( QTcpSocket* socket,QString name,int id )
	{
		users[socket] = name;
		usersID[socket] = id;
	}
	void addAvatar( QTcpSocket* socket, QString nick );
	void setLayoutThread( Layout::LayoutThread* layoutThread );
	void setCoreGraph( Vwr::CoreGraph* cg )
	{
		coreGraph = cg;
	}

	QObject* getCoreWindowReference()
	{
		return cw;
	}

private slots:
	void readyRead();
	void disconnected();

protected:
	void incomingConnection( int socketfd );

private:
	static Server* instance;
	QObject* cw;

	QLinkedList<osg::ref_ptr<Data::Node> > selected_nodes;
	QLinkedList<osg::ref_ptr<Data::Node> > moving_nodes;

	QSet<QTcpSocket*> clients;
	QMap<QTcpSocket*,QString> users;
	QMap<QTcpSocket*,int> usersID;
	QMap<QTcpSocket*,osg::PositionAttitudeTransform*> avatars;
	Layout::LayoutThread* thread;
	Vwr::CoreGraph* coreGraph;
	ExecutorFactory* executorFactory;
	float graphScale;

	QTcpSocket* user_to_spy;
	QTcpSocket* user_to_center;

	osg::Vec3d original_center;
	osg::Quat original_rotation;
	double original_distance;

	QTcpSocket* getClientById( int id );

	quint16 blockSize;
	int avatarScale;

	void sendPlainInstruction( quint8 instruction_number, QTcpSocket* client = NULL );
	void sendBlock( QByteArray block, QTcpSocket* client = NULL );

	void sendColor( quint8 instruction, qlonglong id, float r, float g, float b, float alpha, QTcpSocket* client = NULL );
	QListWidgetItem* getItemById( int id );
};
}

#endif
