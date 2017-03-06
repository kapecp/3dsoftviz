/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Server.h"

#include "Network/Helper.h"
#include "Network/ExecutorFactory.h"
#include "Data/Graph.h"
#include "Manager/Manager.h"
#include "Layout/LayoutThread.h"
#include "Viewer/CoreGraph.h"
#include "Viewer/CameraManipulator.h"

#include "QOSG/CoreWindow.h"

#include "Util/ApplicationConfig.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

Server* Server::instance;

Server::Server( QObject* parent ) :
	QTcpServer( parent ),

	cw( parent ),
	thread( nullptr ),
	coreGraph( nullptr ),
	executorFactory( new ExecutorFactory() ),
	graphScale( Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat() ),
	user_to_spy( nullptr ),
	user_to_center( nullptr ),
	original_distance( 0 ),
	blockSize( 0 ),
	avatarScale( 1 )
{
	instance = this;
}

Server* Server::getInstance()
{
	if ( instance == NULL ) {
		return new Server();
	}
	else {
		return instance;
	}
}

void Server::incomingConnection( int socketfd )
{
	QTcpSocket* client = new QTcpSocket( this );
	client->setSocketDescriptor( socketfd );
	clients.insert( client );

	qDebug() << "New client from:" << client->peerAddress().toString();
	connect( client, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
	connect( client, SIGNAL( disconnected() ), this, SLOT( disconnected() ) );
	connect( client, SIGNAL( disconnected() ), client, SLOT( deleteLater() ) );
}

void Server::readyRead()
{
	QTcpSocket* senderClient = ( QTcpSocket* )sender();
	while ( senderClient->bytesAvailable() ) {

		// vytvorime vstupny stream
		QDataStream in( senderClient );
		in.setFloatingPointPrecision( QDataStream::SinglePrecision );

		// cakame na prichod informacie o velkosti instrukcie
		if ( blockSize == 0 ) {
			if ( senderClient->bytesAvailable() < ( int )sizeof( quint16 ) ) {
				return;
			}

			in >> blockSize;
		}

		// cakame na prichod vsetkych ocakavanych dat
		if ( senderClient->bytesAvailable() < blockSize ) {
			return;
		}

		// poziadame executorFactory o prislusny executor
		AbstractExecutor* executor = executorFactory->getExecutor( &in );

		// a vykoname instrukciu
		if ( executor != NULL && this->isListening() ) {
			executor->execute_server();
		}
		else {
			qDebug() << "Klient: neznama instrukcia";
		}

		// velkost bloku vynulujeme
		blockSize = 0;
	}
}

void Server::disconnected()
{
	QTcpSocket* client = ( QTcpSocket* )sender();
	qDebug() << "Client disconnected:" << client->peerAddress().toString();

	if ( client == user_to_spy ) {
		unSpyUser();
		( ( QOSG::CoreWindow* ) cw )->chb_spy->setChecked( false );
	}
	if ( client == user_to_center ) {
		unCenterUser();
		( ( QOSG::CoreWindow* ) cw )->chb_center->setChecked( false );
	}

	clients.remove( client );
	users.remove( client );
	removeAvatar( client );

	updateUserList();
	sendUserList();
}

void Server::sendUserList()
{
	QStringList userList;

	QMap<QTcpSocket*,QString>::const_iterator i;

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	foreach ( QTcpSocket* client, clients ) {
		userList.clear();
		userList << "0=server";
		for ( i = users.constBegin(); i != users.constEnd(); ++i ) {
			if ( client == i.key() ) {
				continue;
			}
			userList << QString::number( usersID[i.key()] ) + "=" + i.value();
		}
		QString userListJoined = userList.join( "," );

		block.clear();
		out.device()->reset();

		out << ( quint16 )0 << UsersExecutor::INSTRUCTION_NUMBER << userListJoined;
		out.device()->seek( 0 );
		out << ( quint16 )( block.size() - sizeof( quint16 ) );

		this->sendBlock( block, client );
	}
}

void Server::sendGraph( QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}
	/*QTime t;
	t.start();*/

	this->sendPlainInstruction( GraphStartExecutor::INSTRUCTION_NUMBER, client );

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	if ( currentGraph == NULL ) {
		this->sendPlainInstruction( GraphEndExecutor::INSTRUCTION_NUMBER, client );
		return;
	}

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator iNodes =  nodes->constBegin();

	while ( iNodes != nodes->constEnd() ) {

		this->sendNewNode( iNodes.value(),client );

		++iNodes;
	}

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges = currentGraph -> getEdges();
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iEdges =  edges->constBegin();

	while ( iEdges != edges -> constEnd() ) {

		this->sendNewEdge( iEdges.value(),client );

		++iEdges;
	}

	this->sendPlainInstruction( GraphEndExecutor::INSTRUCTION_NUMBER, client );

	//qDebug() << "Sending took" << t.elapsed() << "ms";

}

void Server::sendLayout( QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph == NULL ) {
		return;
	}

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator iNodes =  nodes->constBegin();


	/*QTime t;
	t.start();*/

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	while ( iNodes != nodes->constEnd() ) {

		block.clear();
		out.device()->reset();

		out << ( quint16 )0 << LayoutExecutor::INSTRUCTION_NUMBER
			<< ( int ) iNodes.value()->getId()
			<< ( float )( iNodes.value()->getCurrentPosition().x()/graphScale )
			<< ( float )( iNodes.value()->getCurrentPosition().y()/graphScale )
			<< ( float )( iNodes.value()->getCurrentPosition().z()/graphScale );

		out.device()->seek( 0 );
		out << ( quint16 )( block.size() - sizeof( quint16 ) );

		this->sendBlock( block, client );

		++iNodes;
	}
	//qDebug() << "Sending layout took" << t.elapsed() << "ms";

}

void Server::setLayoutThread( Layout::LayoutThread* layoutThread )
{
	thread = layoutThread;
}

void Server::stopServer()
{

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ServerStopExecutor::INSTRUCTION_NUMBER;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block );

	clients.clear();
	users.clear();

	this->close();

}

void Server::sendMoveNodes()
{
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selected_nodes.constBegin();

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* mergeNodes = NULL;
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	if ( currentGraph != NULL ) {
		mergeNodes = currentGraph->getMetaNodes();
	}


	while ( i != selected_nodes.constEnd() ) {

		block.clear();
		out.device()->reset();

		float x = ( *i )->getCurrentPosition().x();
		float y = ( *i )->getCurrentPosition().y();
		float z = ( *i )->getCurrentPosition().z();

		if ( mergeNodes != NULL && mergeNodes->contains( ( *i )->getId() ) ) {
			x/=graphScale;
			y/=graphScale;
			z/=graphScale;
		}

		out << ( quint16 )0 << MoveNodeExecutor::INSTRUCTION_NUMBER
			<< ( int )( ( *i )->getId() )
			<< ( float ) x
			<< ( float ) y
			<< ( float ) z;
		out.device()->seek( 0 );
		out << ( quint16 )( block.size() - sizeof( quint16 ) );

		++i;

		this->sendBlock( block );
	}

	selected_nodes.clear();
}


void Server::sendMyView( osg::Vec3d center, osg::Quat rotation, double distance, QTcpSocket* client )
{

	QByteArray block;
	QDataStream out( &block, QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << MoveAvatarExecutor::INSTRUCTION_NUMBER << ( float )center.x() << ( float )center.y() << ( float )center.z()
		<< ( float )rotation.x() << ( float )rotation.y() << ( float )rotation.z() << ( float )rotation.w() << ( float )distance << ( int )0;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendMyView( QTcpSocket* client )
{
	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();
	this->sendMyView( cameraManipulator->getCenter(), cameraManipulator->getRotation(), cameraManipulator->getDistance(), client );
}

void Server::sendMyView()
{
	this->sendMyView( NULL );
}

void Server::appendMovingNode( osg::ref_ptr<Data::Node> node )
{
	moving_nodes.append( node );
}

int Server::getMaxUserId()
{
	QList<int> IDs = usersID.values();
	return *( std::max_element( IDs.begin(),IDs.end() ) );
}

void Server::updateUserList()
{

	QListWidget* lw = ( ( QOSG::CoreWindow* ) cw )->lw_users;
	lw->clear();
	QMap<QTcpSocket*,QString>::iterator i = users.begin();
	while ( i != users.end() ) {
		QListWidgetItem* item;
		item = new QListWidgetItem();
		item->setData( 6,usersID[i.key()] );
		item->setText( i.value() );
		lw->addItem( item );
		++i;
	}
}

void Server::removeAvatar( QTcpSocket* client )
{

	osg::PositionAttitudeTransform* pat = avatars.take( client );
	if ( pat != NULL ) {
		pat->removeChildren( 0,2 );
	}
}

QTcpSocket* Server::getClientById( int id )
{
	QMap<QTcpSocket*,int>::iterator i = usersID.begin();
	while ( i != usersID.end() ) {
		if ( i.value() == id ) {
			return i.key();
		}
		++i;
	}
	return NULL;
}

void Server::spyUser( int id )
{

	QTcpSocket* clientToSpy = getClientById( id );

	if ( avatars[clientToSpy] == NULL ) {
		QMessageBox msgBox;
		msgBox.setText( "Can't spy user, which is already spying someone else" );
		msgBox.setIcon( QMessageBox::Information );
		msgBox.setStandardButtons( QMessageBox::Ok );
		msgBox.setDefaultButton( QMessageBox::Ok );
		msgBox.exec();

		( ( QOSG::CoreWindow* ) cw )->chb_spy->setChecked( false );
		return;
	}

	user_to_spy = clientToSpy;

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << SpyUserExecutor::INSTRUCTION_NUMBER << id << ( int )0;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block );

	// store original view
	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();
	original_center = cameraManipulator->getCenter();
	original_rotation = cameraManipulator->getRotation();
	original_distance = cameraManipulator->getDistance();

	setMyView( avatars[user_to_spy]->getPosition(),avatars[user_to_spy]->getAttitude(),original_distance );

	removeAvatar( user_to_spy );
}

void Server::unSpyUser()
{

	if ( users.contains( user_to_spy ) ) {
		addAvatar( user_to_spy, getUserName( user_to_spy ) );
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << UnspyUserExecutor::INSTRUCTION_NUMBER << ( int )getUserId( user_to_spy ) << ( int )0;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block );

	// restore original view
	setMyView( original_center,original_rotation,original_distance );

	sendMyView();

	user_to_spy = NULL;
}

void Server::addAvatar( QTcpSocket* socket, QString nick )
{
	osg::PositionAttitudeTransform* avatar = Helper::generateAvatar( nick );
	avatar->setScale( osg::Vec3d( avatarScale,avatarScale,avatarScale ) );

	QLinkedList<osg::ref_ptr<osg::Node> >* nodes = coreGraph->getCustomNodeList();
	nodes->append( avatar );

	avatars[socket] = avatar;
}

void Server::setMyView( osg::Vec3d center, osg::Quat rotation, double distance )
{
	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();
	cameraManipulator->setCenter( center );
	cameraManipulator->setRotation( rotation );
	cameraManipulator->setDistance( distance );
}

void Server::lookAt( osg::Vec3d coord )
{
	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();
	osg::Quat rotation = Helper::lookAt( cameraManipulator->getCenter(), coord );
	cameraManipulator->setRotation( rotation );
	sendMyView();
}

void Server::centerUser( int id_user )
{

	QTcpSocket* clientToCenter = getClientById( id_user );

	if ( avatars[clientToCenter] == NULL ) {
		QMessageBox msgBox;
		msgBox.setText( "Can't center user, which is spying someone else" );
		msgBox.setIcon( QMessageBox::Information );
		msgBox.setStandardButtons( QMessageBox::Ok );
		msgBox.setDefaultButton( QMessageBox::Ok );
		msgBox.exec();

		( ( QOSG::CoreWindow* ) cw )->chb_center->setChecked( false );
		return;
	}

	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();

	original_distance = cameraManipulator->getDistance();
	original_center = cameraManipulator->getCenter();
	original_rotation = cameraManipulator->getRotation();

	osg::Vec3d direction = original_rotation * osg::Vec3d( 0, 0, 1 );
	direction *= original_distance;
	cameraManipulator->setCenter( cameraManipulator->getCenter()+direction );
	cameraManipulator->setDistance( 0 );

	user_to_center = clientToCenter;
	osg::PositionAttitudeTransform* userAvatar = avatars[user_to_center];
	lookAt( userAvatar->getPosition() );
}

void Server::unCenterUser()
{
	user_to_center = NULL;
	setMyView( original_center,original_rotation,original_distance );
	sendMyView();
}

void Server::sendNewNode( osg::ref_ptr<Data::Node> node, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	osg::Vec4 color = node->getColor();

	out     << ( quint16 )0 << NewNodeExecutor::INSTRUCTION_NUMBER
			<< ( int ) node->getId()
			<< ( float )( node->getCurrentPosition().x()/graphScale )
			<< ( float )( node->getCurrentPosition().y()/graphScale )
			<< ( float )( node->getCurrentPosition().z()/graphScale )
			<< ( QString )( ( ( Data::AbsNode* ) node )->getName() )
			<< ( float ) color.x()
			<< ( float ) color.y()
			<< ( float ) color.z()
			<< ( float ) color.w();

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendNewEdge( osg::ref_ptr<Data::Edge> edge, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out    << ( quint16 )0 << NewEdgeExecutor::INSTRUCTION_NUMBER
		   << ( int ) edge->getId()
		   << ( int )( edge->getSrcNode()->getId() )
		   << ( int )( edge->getDstNode()->getId() )
		   << ( bool ) edge->isOriented();

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );

}

void Server::sendRemoveNode( qlonglong id, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 ) RemoveNodeExecutor::INSTRUCTION_NUMBER << ( int )id;
	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendRemoveEdge( qlonglong id, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 ) RemoveEdgeExecutor::INSTRUCTION_NUMBER << ( int )id;
	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}


void Server::sendNodeColor( qlonglong id, float r, float g, float b, float alpha, QTcpSocket* client )
{
	this->sendColor( SetNodeColorExecutor::INSTRUCTION_NUMBER, id, r, g, b, alpha, client );
}

void Server::sendEdgeColor( qlonglong id, float r, float g, float b, float alpha, QTcpSocket* client )
{
	this->sendColor( SetEdgeColorExecutor::INSTRUCTION_NUMBER, id, r, g, b, alpha, client );
}

void Server::sendColor( quint8 instruction, qlonglong id, float r, float g, float b, float alpha, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) instruction
		<< ( int ) id << ( float ) r << ( float ) g << ( float ) b << ( float ) alpha;
	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendNodeLabel( qlonglong id, QString label, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 ) SetNodeLabelExecutor::INSTRUCTION_NUMBER << ( int ) id << ( QString ) label;
	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendFixNodeState( qlonglong id, bool state, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 ) SetFixNodeStateExecutor::INSTRUCTION_NUMBER << ( int ) id << ( bool ) state;
	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendMergeNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, osg::Vec3f position, qlonglong mergeNodeId, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) MergeNodesExecutor::INSTRUCTION_NUMBER << ( int ) selectedNodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator iAdd = selectedNodes->constBegin();
	while ( iAdd != selectedNodes->constEnd() ) {
		out << ( int )( *iAdd )->getId();
		++iAdd;
	}

	out << ( int ) mergeNodeId;

	out << ( float ) position.x() << ( float ) position.y() << ( float ) position.z();

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendSeparateNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 ) SeparateNodesExecutor::INSTRUCTION_NUMBER << ( int ) selectedNodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator iAdd = selectedNodes->constBegin();
	while ( iAdd != selectedNodes->constEnd() ) {
		out << ( int )( *iAdd )->getId();
		++iAdd;
	}

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendAddMetaNode( osg::ref_ptr<Data::Node> metaNode, QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, QString edgeName, osg::Vec3f position, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) AddMetaNodeExecutor::INSTRUCTION_NUMBER << ( int ) metaNode->getId() << ( QString )( ( Data::AbsNode* )metaNode )->getName();
	out << ( float ) position.x() << ( float ) position.y() << ( float ) position.z();
	out << ( QString ) edgeName;
	out << ( int ) selectedNodes->count();

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges = metaNode->getEdges();
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator i = edges->constBegin();
	while ( i != edges->constEnd() ) {
		out << ( int )( *i )->getId() << ( int )( *i )->getSrcNode()->getId();
		++i;
	}


	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendSetRestriction( quint8 type, osg::ref_ptr<Data::Node> node1, osg::Vec3 position_node1,  osg::ref_ptr<Data::Node> node2, osg::Vec3 position_node2, QLinkedList<osg::ref_ptr<Data::Node> >* nodes, osg::ref_ptr<Data::Node> node3, osg::Vec3* position_node3, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) SetRestrictionExecutor::INSTRUCTION_NUMBER << ( quint8 ) type;
	out << ( int ) node1->getId() << ( QString )( ( Data::AbsNode* ) node1 )->getName();
	out << ( float ) position_node1.x() << ( float ) position_node1.y() << ( float ) position_node1.z();
	out << ( int ) node2->getId() << ( QString )( ( Data::AbsNode* ) node2 )->getName();
	out << ( float ) position_node2.x() << ( float ) position_node2.y() << ( float ) position_node2.z();

	if ( type == 3 && node3 != NULL && position_node3 != NULL ) {
		out << ( int ) node3->getId() << ( QString )( ( Data::AbsNode* ) node3 )->getName();
		out << ( float ) position_node3->x() << ( float ) position_node3->y() << ( float ) position_node3->z();
	}

	out << ( int ) nodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();
	while ( i != nodes->constEnd() ) {
		out << ( int )( *i )->getId();
		++i;
	}


	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendSetRestriction( quint8 type, QLinkedList<osg::ref_ptr<Data::Node> >* nodes,
								 Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType* restrictionNodes,
								 QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) SetRestrictionExecutor::INSTRUCTION_NUMBER << ( quint8 ) type;

	Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType::Iterator itRN;

	for ( itRN = restrictionNodes->begin(); itRN!= restrictionNodes->end(); itRN++ ) {
		out << ( int )( *itRN )->getId() << ( QString )( ( Data::AbsNode* )( *itRN ) )->getName();
		out<< ( float )( *itRN )->getTargetPosition().x()<< ( float )( *itRN )->getTargetPosition().y() << ( float )( *itRN )->getTargetPosition().z();
	}

	out << ( int ) nodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();
	while ( i != nodes->constEnd() ) {
		out << ( int )( *i )->getId();
		++i;
	}


	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendUnSetRestriction( QLinkedList<osg::ref_ptr<Data::Node> >* nodes, QTcpSocket* client )
{

	if ( !this -> isListening() || ( client == NULL && clients.size() == 0 ) ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out << ( quint16 )0 << ( quint8 ) UnSetRestrictionExecutor::INSTRUCTION_NUMBER;

	out << ( int ) nodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();
	while ( i != nodes->constEnd() ) {
		out << ( int )( *i )->getId();
		++i;
	}


	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::sendPlainInstruction( quint8 instruction_number, QTcpSocket* client )
{

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 )instruction_number;
	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );

}

void Server::sendBlock( QByteArray block, QTcpSocket* client )
{

	if ( client == NULL ) {
		foreach ( QTcpSocket* otherClient, clients ) {
			otherClient->write( block );
		}
	}
	else {
		client -> write( block );
	}

}

void Server::setAttention( int user )
{
	QListWidgetItem* item = this->getItemById( user );
	if ( item != NULL ) {
		item->setIcon( QIcon( "img/gui/attention.png" ) );
	}
}

void Server::unSetAttention( int user )
{
	QListWidgetItem* item = this->getItemById( user );
	if ( item != NULL ) {
		item->setIcon( QIcon() );
	}
}

QListWidgetItem* Server::getItemById( int id )
{
	QOSG::CoreWindow* coreWindow = ( QOSG::CoreWindow* ) cw;
	int count = coreWindow->lw_users->count();

	for ( int i = 0; i < count; i++ ) {
		QListWidgetItem* item = coreWindow->lw_users->item( i );
		if ( item->data( 6 ) == id ) {
			return item;
		}
	}
	return NULL;
}

void Server::sendAttractAttention( bool attention, int idUser, QTcpSocket* client )
{
	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 )AttractAttentionExecutor::INSTRUCTION_NUMBER << ( bool ) attention << ( int )idUser;
	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	this->sendBlock( block, client );
}

void Server::setAvatarScale( int scale )
{
	avatarScale = scale;
	foreach ( osg::PositionAttitudeTransform* avatar, avatars ) {
		avatar->setScale( osg::Vec3d( avatarScale,avatarScale,avatarScale ) );
	}
}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
