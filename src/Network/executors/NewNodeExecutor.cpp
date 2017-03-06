#include "Network/executors/NewNodeExecutor.h"
#include "Manager/Manager.h"
#include "Importer/GraphOperations.h"
#include "Network/Server.h"
#include "Network/Client.h"


#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif


namespace Network {

void NewNodeExecutor::execute_client()
{

	Client* client = Client::getInstance();
	client->ignoreLayout( true );

	int id;

	float x;
	float y;
	float z;

	float color_x, color_y, color_z, color_w;

	QString name;

	*stream >> id >> x >> y >> z >> name >> color_x >> color_y >> color_z >> color_w;
	//qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

	osg::Vec3 position( x,y,z );
	osg::ref_ptr<Data::Node> newNode  = client->currentGraph->addNode( id,name, client->nodeType, position );

	newNode->setColor( osg::Vec4( color_x, color_y, color_z, color_w ) );
	client->ignoreLayout( false );
}

void NewNodeExecutor::execute_server()
{

	Server* server = Server::getInstance();

	float x;
	float y;
	float z;

	QString name;

	*stream >> x >> y >> z >> name;
	//qDebug()<< "[NEW NODE]" << "[" << x << "," << y << "," << z << "]";

	osg::Vec3 position( x/server->getGraphScale(),y/server->getGraphScale(),z/server->getGraphScale() );
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	if ( currentGraph == NULL ) {
		currentGraph = Manager::GraphManager::getInstance()->createNewGraph( "New graph" );
	}

	Data::Type* nodeType;
	Data::Type* edgeType;
	Importer::GraphOperations* operations = new Importer::GraphOperations( *currentGraph );
	operations->addDefaultTypes( edgeType, nodeType );

	osg::ref_ptr<Data::Node> newNode  = currentGraph->addNode( name, nodeType, position );

	if ( ( ( QOSG::CoreWindow* )server->getCoreWindowReference() )->playing() ) {
		server->getLayoutThread()->play();
	}

	server->sendNewNode( newNode );
	delete operations;
}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
