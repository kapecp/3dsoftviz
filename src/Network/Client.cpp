/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"

//#include "Data/Graph.h"

#include "Network/Helper.h"
#include "Network/ExecutorFactory.h"

#include "QOSG/CoreWindow.h"

#include "Layout/LayoutThread.h"

#include "Viewer/CameraManipulator.h"
#include "Viewer/CoreGraph.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

Client* Client::instance;

Client::Client( QObject* parent ) : QObject( parent )
{

	instance = this;

	socket = new QTcpSocket( this );

	cw = parent;

	connect( socket, SIGNAL( readyRead() ), this, SLOT( readyRead() ) );
	connect( socket, SIGNAL( connected() ), this, SLOT( connected() ) );
	connect( socket, SIGNAL( error( QAbstractSocket::SocketError ) ), this, SLOT( error() ) );

	edgeType = NULL;
	nodeType = NULL;
	user_to_spy = -1;
	user_to_center = -1;
	layoutIgnore = true;

	executorFactory = new ExecutorFactory();

	blockSize = 0;
	avatarScale = 1;

}

Client* Client::getInstance()
{
	if ( instance == NULL ) {
		return new Client();
	}
	else {
		return instance;
	}
}

void Client::ServerConnect( QString nick, QString address )
{
	clientNick = nick;
	socket->connectToHost( address, 4200 );
}

void Client::send_message( QString message )
{
	// todo: implement

	qDebug() << "Client:send message: "+ message;
}

void Client::requestGraph()
{
	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << SendGraphExecutor::INSTRUCTION_NUMBER;
	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );
}

void Client::readyRead()
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
		if ( executor != NULL && this->isConnected() ) {
			executor->execute_client();
		}
		else {
			qDebug() << "Klient: neznama instrukcia";
		}

		// velkost bloku vynulujeme
		blockSize = 0;
	}
}

void Client::connected()
{

	QByteArray block;
	QDataStream out( &block, QIODevice::WriteOnly );

	out << ( quint16 )0;
	out << IncommingUserExecutor::INSTRUCTION_NUMBER;
	out << clientNick;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

	( ( QOSG::CoreWindow* ) cw ) -> le_client_name -> setEnabled( false );
	( ( QOSG::CoreWindow* ) cw ) -> le_server_addr -> setEnabled( false );
	( ( QOSG::CoreWindow* ) cw ) -> b_start_client -> setEnabled( true );
	( ( QOSG::CoreWindow* ) cw ) -> b_start_client -> setText( "Disconnect" );
}

void Client::error()
{
	this->disconnect();
}

void Client::disconnected()
{
	this->disconnect();
}

void Client::disconnect()
{
	if ( socket -> isOpen() ) {
		socket -> disconnectFromHost();
		socket -> close();
	}

	if ( user_to_spy != -1 ) {
		unSpyUser();
		( ( QOSG::CoreWindow* ) cw )->chb_spy->setChecked( false );
	}
	if ( user_to_center != -1 ) {
		unCenterUser();
		( ( QOSG::CoreWindow* ) cw )->chb_center->setChecked( false );
	}

	QMap<int, QString>::iterator i = userList.begin();
	while ( i != userList.end() ) {
		removeAvatar( i.key() );
		++i;
	}
	userList.clear();
	updateUserList();

	( ( QOSG::CoreWindow* ) cw ) -> le_client_name -> setEnabled( true );
	( ( QOSG::CoreWindow* ) cw ) -> le_server_addr -> setEnabled( true );
	( ( QOSG::CoreWindow* ) cw ) -> b_start_client -> setEnabled( true );
	( ( QOSG::CoreWindow* ) cw ) -> b_start_client -> setText( "Connect to session" );
	( ( QOSG::CoreWindow* ) cw ) -> b_start_server -> setEnabled( true );
}

void Client::setLayoutThread( Layout::LayoutThread* layoutThread )
{
	thread = layoutThread;
}

bool Client::isConnected()
{
	return socket -> state() == QAbstractSocket::ConnectedState;
}

void Client::sendMovedNodesPosition()
{
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selected_nodes.constBegin();

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	while ( i != selected_nodes.constEnd() ) {

		block.clear();
		out.device()->reset();

		out << ( quint16 )0 << MoveNodeExecutor::INSTRUCTION_NUMBER
			<< ( int )( ( *i )->getId() )
			<< ( float )( ( *i )->getCurrentPosition().x() )
			<< ( float )( ( *i )->getCurrentPosition().y() )
			<< ( float )( ( *i )->getCurrentPosition().z() );
		out.device()->seek( 0 );
		out << ( quint16 )( block.size() - sizeof( quint16 ) );

		++i;

		socket->write( block );
	}

	selected_nodes.clear();
}

void Client::sendMyView( osg::Vec3d center, osg::Quat rotation, float distance )
{

	QByteArray block;
	QDataStream out( &block, QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << MoveAvatarExecutor::INSTRUCTION_NUMBER << ( float )center.x() << ( float )center.y() << ( float )center.z()
		<< ( float )rotation.x() << ( float )rotation.y() << ( float )rotation.z() << ( float )rotation.w() << ( float ) distance;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );
}

void Client::sendMyView()
{
	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();

//	this->sendMyView( cameraManipulator->getCenter(), cameraManipulator->getRotation(),  ( float ) cameraManipulator->getDistance() );

	this->sendMyView( cameraManipulator->getCenter(), cameraManipulator->getRotation(), static_cast<float>( cameraManipulator->getDistance() ) );
//>>>>>>> Stashed changes
}

void Client::updateUserList()
{
	QListWidget* lw = ( ( QOSG::CoreWindow* ) cw )->lw_users;
	lw->clear();
	QMap<int,QString>::iterator i = userList.begin();
	while ( i != userList.end() ) {
		QListWidgetItem* item;
		item = new QListWidgetItem();
		item->setData( 6,i.key() );
		item->setText( i.value() );
		lw->addItem( item );
		++i;
	}
}

void Client::setMyView( osg::Vec3d center, osg::Quat rotation, double distance )
{
	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();
	cameraManipulator->setCenter( center );
	cameraManipulator->setRotation( rotation );
	cameraManipulator->setDistance( distance );
}

void Client::addClient( int id, QString nick )
{
	if ( !userList.contains( id ) ) {
		userList.insert( id,nick );
		addAvatar( id,nick );
	}
}

void Client::addAvatar( int id, QString nick )
{

	osg::PositionAttitudeTransform* PAtransform = Helper::generateAvatar( nick );
	PAtransform->setScale( osg::Vec3d( avatarScale,avatarScale,avatarScale ) );

	QLinkedList<osg::ref_ptr<osg::Node> >* nodes = coreGraph->getCustomNodeList();

	nodes->append( PAtransform );

	//PAtransform->setScale(osg::Vec3d(10,10,10));
	avatarList.insert( id,PAtransform );
}

void Client::removeAvatar( int id )
{
	osg::PositionAttitudeTransform* pat = avatarList.take( id );
	if ( pat != NULL ) {
		pat->removeChild( 0,2 );
	}
}

void Client::showClientAvatar( int id )
{
	addAvatar( id, userList[id] );
}

void Client::unSpyUser()
{
	if ( userList.contains( user_to_spy ) ) {
		showClientAvatar( user_to_spy );
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << UnspyUserExecutor::INSTRUCTION_NUMBER << ( int )user_to_spy;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

	// restore original view
	setMyView( original_center,original_rotation,original_distance );

	sendMyView();
	user_to_spy = -1;
}

void Client::spyUser( int user )
{

	if ( avatarList[user] == NULL ) {
		QMessageBox msgBox;
		msgBox.setText( "Can't spy user, which is already spying someone else" );
		msgBox.setIcon( QMessageBox::Information );
		msgBox.setStandardButtons( QMessageBox::Ok );
		msgBox.setDefaultButton( QMessageBox::Ok );
		msgBox.exec();

		( ( QOSG::CoreWindow* ) cw )->chb_spy->setChecked( false );
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << SpyUserExecutor::INSTRUCTION_NUMBER << user;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );
	user_to_spy = user;

	// store original view
	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();
	original_center = cameraManipulator->getCenter();
	original_rotation = cameraManipulator->getRotation();
	original_distance = cameraManipulator->getDistance();

	setMyView( avatarList[user]->getPosition(),avatarList[user]->getAttitude(),original_distance );
	removeAvatar( user );
}

void Client::lookAt( osg::Vec3d coord )
{
	Vwr::CameraManipulator* cameraManipulator = ( ( QOSG::CoreWindow* ) cw )->getCameraManipulator();

	osg::Quat rotation = Helper::lookAt( cameraManipulator->getCenter(), coord );
	cameraManipulator->setRotation( rotation );
	sendMyView();
}

void Client::centerUser( int id_user )
{

	if ( avatarList[id_user] == NULL ) {
		QMessageBox msgBox;
		msgBox.setText( "Can't center user, which is already spying someone else" );
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

	user_to_center = id_user;
	osg::PositionAttitudeTransform* userAvatar = avatarList[id_user];
	lookAt( userAvatar->getPosition() );
}

void Client::unCenterUser()
{
	user_to_center = -1;
	setMyView( original_center,original_rotation,original_distance );
	sendMyView();
}

void Client::sendNewNode( QString name, osg::Vec3f position )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out     << ( quint16 )0 << NewNodeExecutor::INSTRUCTION_NUMBER
			<< ( float )( position.x() )
			<< ( float )( position.y() )
			<< ( float )( position.z() )
			<< ( QString )( name );

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );
}

void Client::sendNewEdge( QString name, qlonglong id_from, qlonglong id_to, bool oriented )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out     << ( quint16 )0 << NewEdgeExecutor::INSTRUCTION_NUMBER
			<< ( QString )( name )
			<< ( int )( id_from )
			<< ( int )( id_to )
			<< ( bool )( oriented );

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendRemoveNode( qlonglong id )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out     << ( quint16 )0 << RemoveNodeExecutor::INSTRUCTION_NUMBER
			<< ( int )( id );

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendRemoveEdge( qlonglong id )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out     << ( quint16 )0 << RemoveEdgeExecutor::INSTRUCTION_NUMBER
			<< ( int )( id );

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}


void Client::sendNodeColor( qlonglong id, float r, float g, float b, float alpha )
{
	this->sendColor( SetNodeColorExecutor::INSTRUCTION_NUMBER, id, r, g, b, alpha );
}

void Client::sendEdgeColor( int id, float r, float g, float b, float alpha )
{
	this->sendColor( SetEdgeColorExecutor::INSTRUCTION_NUMBER, id, r, g, b, alpha );
}

void Client::sendColor( quint8 instruction, qlonglong id, float r, float g, float b, float alpha )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out     << ( quint16 )0 << instruction
			<< ( int )( id ) << ( float ) r << ( float ) g << ( float ) b << ( float ) alpha;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendNodeLabel( qlonglong id, QString label )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 ) SetNodeLabelExecutor::INSTRUCTION_NUMBER << ( int )( id ) << ( QString ) label;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendFixNodeState( qlonglong id, bool state )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );

	out << ( quint16 )0 << ( quint8 ) SetFixNodeStateExecutor::INSTRUCTION_NUMBER << ( int )( id ) << ( bool ) state;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendMergeNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, osg::Vec3f position )
{

	if ( !this -> isConnected() ) {
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

	out << ( float ) position.x() << ( float ) position.y() << ( float ) position.z();

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendSeparateNodes( QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) SeparateNodesExecutor::INSTRUCTION_NUMBER << ( int ) selectedNodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator iAdd = selectedNodes->constBegin();
	while ( iAdd != selectedNodes->constEnd() ) {
		out << ( int )( *iAdd )->getId();
		++iAdd;
	}

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendAddMetaNode( QString name, QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes, QString edgeName, osg::Vec3f position )
{

	if ( !this -> isConnected() ) {
		return;
	}

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) AddMetaNodeExecutor::INSTRUCTION_NUMBER << ( QString ) name;
	out << ( float ) position.x() << ( float ) position.y() << ( float ) position.z();
	out << ( QString ) edgeName;
	out << ( int ) selectedNodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator iAdd = selectedNodes->constBegin();
	while ( iAdd != selectedNodes->constEnd() ) {
		out << ( int )( *iAdd )->getId();
		++iAdd;
	}

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendSetRestriction( quint8 type, QString name_node1, osg::Vec3 position_node1, QString name_node2, osg::Vec3 position_node2, QLinkedList<osg::ref_ptr<Data::Node> >* nodes, QString name_node3, osg::Vec3* position_node3 )
{
	if ( !this -> isConnected() ) {
		return;
	}
	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) SetRestrictionExecutor::INSTRUCTION_NUMBER << ( quint8 ) type;
	out << ( QString ) name_node1 << ( float ) position_node1.x() << ( float ) position_node1.y() << ( float ) position_node1.z();
	out << ( QString ) name_node2 << ( float ) position_node2.x() << ( float ) position_node2.y() << ( float ) position_node2.z();

	if ( type == 3 && position_node3 != NULL ) {
		out << ( QString ) name_node3 << ( float ) position_node3->x() << ( float ) position_node3->y() << ( float ) position_node3->z();
	}

	out << ( int ) nodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();
	while ( i != nodes->constEnd() ) {
		out << ( int )( *i )->getId();
		++i;
	}

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendSetRestriction( quint8 type, QLinkedList<osg::ref_ptr<Data::Node> >* nodes,
								 Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType* restrictionNodes )
{
	if ( !this -> isConnected() ) {
		return;
	}
	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << ( quint8 ) SetRestrictionExecutor::INSTRUCTION_NUMBER << ( quint8 ) type;

	Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType::Iterator itRN;

	for ( itRN = restrictionNodes->begin(); itRN!= restrictionNodes->end(); itRN++ ) {
		out << ( QString )( ( Data::AbsNode* )( *itRN ) )->getName() << ( float )( *itRN )->getTargetPosition().x()<< ( float )( *itRN )->getTargetPosition().y() << ( float )( *itRN )->getTargetPosition().z();
	}

	out << ( int ) nodes->count();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();
	while ( i != nodes->constEnd() ) {
		out << ( int )( *i )->getId();
		++i;
	}

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );

}

void Client::sendUnSetRestriction( QLinkedList<osg::ref_ptr<Data::Node> >* nodes )
{
	if ( !this -> isConnected() ) {
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

	socket->write( block );
}

void Client::setAttention( int user )
{
	QListWidgetItem* item = this->getItemById( user );
	if ( item != NULL ) {
		item->setIcon( QIcon( "img/gui/attention.png" ) );
	}
}

void Client::unSetAttention( int user )
{
	QListWidgetItem* item = this->getItemById( user );
	if ( item != NULL ) {
		item->setIcon( QIcon() );
	}
}

QListWidgetItem* Client::getItemById( int id )
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

void Client::sendAttractAttention( bool attention )
{
	if ( !this -> isConnected() ) {
		return;
	}
	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out << ( quint16 )0 << ( quint8 ) AttractAttentionExecutor::INSTRUCTION_NUMBER << ( bool ) attention;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	socket->write( block );
}

void Client::setAvatarScale( int scale )
{
	avatarScale = scale;
	foreach ( osg::PositionAttitudeTransform* avatar, avatarList ) {
		avatar->setScale( osg::Vec3d( avatarScale,avatarScale,avatarScale ) );
	}
}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
