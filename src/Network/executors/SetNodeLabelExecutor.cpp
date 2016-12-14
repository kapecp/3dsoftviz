#include "Network/executors/SetNodeLabelExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

namespace Network {

void SetNodeLabelExecutor::execute_client()
{

	int id;
	QString label;

	*stream >> id >> label;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	if ( nodes->contains( id ) ) {
		Data::Node* node = *nodes->find( id );
		( ( Data::AbsNode* )node )->setName( label );
		node->setLabelText( label );
		node->reloadConfig();
	}

}

void SetNodeLabelExecutor::execute_server()
{

	int id;
	QString label;

	*stream >> id >> label;

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
	if ( nodes->contains( id ) ) {
		Data::Node* node = *nodes->find( id );
		( ( Data::AbsNode* )node )->setName( label );
		node->setLabelText( label );
		node->reloadConfig();
	}

	Server* server = Server::getInstance();
	server->sendNodeLabel( id, label );

}

} // namespace Network

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
