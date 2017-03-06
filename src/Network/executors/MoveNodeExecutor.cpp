#include "Network/executors/MoveNodeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"
#include "Network/Client.h"
#include "Data/Node.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void MoveNodeExecutor::execute_client()
{

	int id;
	float x,y,z;

	*stream >> id >> x >> y >> z;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	//qDebug() << "Moving" << id << "to" << x << y << z;
	if ( nodes->contains( id ) ) {
		this->moveNode( *nodes->find( id ), osg::Vec3( x,y,z ) );
	}
	else {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* mergeNodes = currentGraph->getMetaNodes();
		if ( mergeNodes->contains( id ) ) {
			this->moveNode( *mergeNodes->find( id ), osg::Vec3( x,y,z ) );
		}
		else {
			qDebug() << "Nepoznam uzol" << id;
		}
	}

	//TODO: refactor
	// Stupid way to fix restriction shape refreshing
	( ( QOSG::CoreWindow* )Client::getInstance()->getCoreWindowReference() )->getLayoutThread()->pauseAllAlg();
	( ( QOSG::CoreWindow* )Client::getInstance()->getCoreWindowReference() )->getLayoutThread()->play();
}

void MoveNodeExecutor::execute_server()
{

	int id;
	float x,y,z;

	Server* server = Server::getInstance();
	float graphScale = server->getGraphScale();
	QSet<QTcpSocket*> clients = server->getClients();

	*stream >> id >> x >> y >> z;

	x/=graphScale;
	y/=graphScale;
	z/=graphScale;

	QByteArray block;
	QDataStream out( &block,QIODevice::WriteOnly );
	out.setFloatingPointPrecision( QDataStream::SinglePrecision );

	out << ( quint16 )0 << MoveNodeExecutor::INSTRUCTION_NUMBER << id << x << y << z;

	out.device()->seek( 0 );
	out << ( quint16 )( block.size() - sizeof( quint16 ) );

	foreach ( QTcpSocket* otherClient, clients ) {
		otherClient->write( block );
	}

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	//qDebug() << "Moving" << id << "to" << x << y << z;

	if ( nodes->contains( id ) ) {
		this->moveNode( *nodes->find( id ), osg::Vec3( x,y,z ) );
		Layout::LayoutThread* thread = server->getLayoutThread();
		thread->wakeUp();
	}
	else {
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* mergedNodes = currentGraph->getMetaNodes();
		if ( mergedNodes->contains( id ) ) {
			this->moveNode( *mergedNodes->find( id ), osg::Vec3( x,y,z ) );
			Layout::LayoutThread* thread = server->getLayoutThread();
			thread->wakeUp();
		}
		else {
			qDebug() << "Nepoznam uzol" << id;
		}
	}

}

void MoveNodeExecutor::moveNode( Data::Node* node, osg::Vec3 position )
{
	if ( Client::getInstance()->selected_nodes.contains( node ) ) {
		return;
	}

	node -> setUsingInterpolation( false );
	node -> setFixed( true );
	node -> setTargetPosition( position );
}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
