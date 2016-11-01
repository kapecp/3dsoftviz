#include "Network/executors/NewEdgeExecutor.h"

#include "Manager/Manager.h"
#include "Network/Server.h"
#include "Network/Client.h"
#include "Data/GraphLayout.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void NewEdgeExecutor::execute_client()
{

	Client* client = Client::getInstance();

	int id;
	int from;
	int to;
	bool oriented;

	*stream >> id >> from >> to >> oriented;

	//qDebug()<< "[NEW EDGE] id: " << id << " from: " << from << ", to:" << to;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();

	osg::ref_ptr<Data::Node> node_from = *nodes->find( from );
	osg::ref_ptr<Data::Node> node_to = *nodes->find( to );

	client->currentGraph->addEdge( id,"NewEdge",node_from,node_to,client->edgeType,oriented );
}

void NewEdgeExecutor::execute_server()
{

	Server* server = Server::getInstance();

	QString name;
	int from, to;
	bool oriented;

	*stream >> name >> from >> to >> oriented;
	//qDebug()<< "[NEW NODE]" << "[" << x << "," << y << "," << z << "]";


	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();

	osg::ref_ptr<Data::Node> node_from = *nodes->find( from );
	osg::ref_ptr<Data::Node> node_to = *nodes->find( to );

	Data::Type* type = currentGraph->addType( Data::GraphLayout::META_EDGE_TYPE );

	osg::ref_ptr<Data::Edge> newEdge = currentGraph->addEdge( "NewEdge",node_from,node_to,type,oriented );

	if ( ( ( QOSG::CoreWindow* )server->getCoreWindowReference() )->playing() ) {
		server->getLayoutThread()->play();
	}

	server->sendNewEdge( newEdge );
}

}

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
