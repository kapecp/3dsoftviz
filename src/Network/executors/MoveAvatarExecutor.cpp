#include "Network/executors/MoveAvatarExecutor.h"
#include "Network/Server.h"
#include "Network/Client.h"


#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif


namespace Network {

void MoveAvatarExecutor::execute_client()
{

	Client* client = Client::getInstance();

	double x,y,z,a,b,c,d,distance;
	int id;

	*stream >> x >> y >> z >> a >> b >> c >> d >> distance >> id;

	osg::Vec3d center = osg::Vec3d( x-5,y,z );
	osg::Quat rotation = osg::Quat( a,b,c,d );

	osg::Vec3d direction = rotation * osg::Vec3d( 0, 0, 1 );
	direction *= distance;

	if ( client->userToSpy() != id ) {
		osg::PositionAttitudeTransform* PAtransform = client->avatarList[id];
		if ( PAtransform != NULL ) {
			PAtransform->setAttitude( rotation );
			PAtransform->setPosition( center+direction );
		}
		else {
			//qDebug() << "Nepoznam avatar" << id;
		}
	}

	if ( client->userToSpy() == id ) {
		client->setMyView( center,rotation,distance );
	}

	if ( client->getCenterUser() == id ) {
		client->lookAt( center+direction );
	}

}

void MoveAvatarExecutor::execute_server()
{

	Server* server = Server::getInstance();

	QTcpSocket* out_socket = ( QTcpSocket* ) stream->device();

	double x,y,z,a,b,c,d,distance;
	*stream >> x >> y >> z >> a >> b >> c >> d >> distance;

	QSet<QTcpSocket*> clients = server->getClients();

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	int sender_id = server->getUserId( out_socket );

	out << ( quint16 )0 << MoveAvatarExecutor::INSTRUCTION_NUMBER << x << y << z << a << b << c << d << distance << sender_id;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	foreach ( QTcpSocket* client, clients ) { // append sender ID and resend to all other clients except sender
		if ( client == out_socket ) {
			continue;
		}
		client->write( block );
	}

	osg::Vec3d center = osg::Vec3d( x-5,y,z );
	osg::Quat rotation = osg::Quat( a,b,c,d );

	osg::Vec3d direction = rotation * osg::Vec3d( 0, 0, 1 );
	direction *= distance;

	osg::PositionAttitudeTransform* PAtransform = server->getAvatarTransform( out_socket );

	if ( PAtransform != NULL ) {
		PAtransform->setAttitude( rotation );
		PAtransform->setPosition( center+direction );
	}

	if ( server->userToSpy() == out_socket ) {
		server->setMyView( center,rotation,distance );
	}

	if ( server->getCenterUser() == out_socket ) {
		server->lookAt( center+direction );
	}

}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
