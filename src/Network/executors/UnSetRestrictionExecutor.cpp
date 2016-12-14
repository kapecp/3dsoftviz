#include "Network/executors/UnSetRestrictionExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"
#include "Network/Client.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void UnSetRestrictionExecutor::execute_client()
{
	int id;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	Client* client = Client::getInstance();
	QOSG::CoreWindow* cw = ( QOSG::CoreWindow* ) client->getCoreWindowReference();

	QLinkedList<osg::ref_ptr<Data::Node> >* nodesToUnRestrict = new QLinkedList<osg::ref_ptr<Data::Node> >();

	int count;

	*stream >> count;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph->getNodes();
	for ( int i = 0; i < count; i++ ) {
		*stream >> id;
		if ( nodes->contains( id ) ) {
			nodesToUnRestrict->append( *nodes->find( id ) );
		}
	}


	cw->setRestrictionToSelectedNodes(
		QSharedPointer<Layout::ShapeGetter> ( NULL ),
		currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler> ( NULL ),
		nodesToUnRestrict
	);

}

void UnSetRestrictionExecutor::execute_server()
{
	int id;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	Client* client = Client::getInstance();
	QOSG::CoreWindow* cw = ( QOSG::CoreWindow* ) client->getCoreWindowReference();

	QLinkedList<osg::ref_ptr<Data::Node> >* nodesToUnRestrict = new QLinkedList<osg::ref_ptr<Data::Node> >();

	int count;

	*stream >> count;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph->getNodes();
	for ( int i = 0; i < count; i++ ) {
		*stream >> id;
		if ( nodes->contains( id ) ) {
			nodesToUnRestrict->append( *nodes->find( id ) );
		}
	}


	cw->setRestrictionToSelectedNodes(
		QSharedPointer<Layout::ShapeGetter> ( NULL ),
		currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler> ( NULL ),
		nodesToUnRestrict
	);
	Server* server = Server::getInstance();
	server->sendUnSetRestriction( nodesToUnRestrict );
}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
