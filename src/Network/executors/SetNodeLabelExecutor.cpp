#include "Network/executors/SetNodeLabelExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

#include <leathers/push>
#include <leathers/useless-cast>
#include <leathers/old-style-cast>
#include <leathers/sign-conversion>

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

#include <leathers/pop>
