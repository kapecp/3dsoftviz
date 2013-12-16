#include "Network/executors/UnSetRestrictionExecutor.h"
#include "Manager/Manager.h"
#include "Importer/GraphOperations.h"
#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"
#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
#include "Layout/ShapeGetter_Sphere_ByTwoNodes.h"
#include "Layout/ShapeGetter_Plane_ByThreeNodes.h"
#include "Network/Server.h"

using namespace Network;

void UnSetRestrictionExecutor::execute_client() {
	int id;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	Client * client = Client::getInstance();
    QOSG::CoreWindow * cw = (QOSG::CoreWindow *) client->getCoreWindowReference();

	QLinkedList<osg::ref_ptr<Data::Node> > * nodesToUnRestrict = new QLinkedList<osg::ref_ptr<Data::Node> >();

    int count;

    *stream >> count;

    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph->getNodes();
    for (int i = 0; i < count; i++) {
        *stream >> id;
        if (nodes->contains(id)) {
			nodesToUnRestrict->append(*nodes->find(id));
        }
    }


	cw->setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> (NULL),
				currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler> (NULL),
				nodesToUnRestrict
				);

}

void UnSetRestrictionExecutor::execute_server() {
	int id;

	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	Client * client = Client::getInstance();
	QOSG::CoreWindow * cw = (QOSG::CoreWindow *) client->getCoreWindowReference();

	QLinkedList<osg::ref_ptr<Data::Node> > * nodesToUnRestrict = new QLinkedList<osg::ref_ptr<Data::Node> >();

	int count;

	*stream >> count;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph->getNodes();
	for (int i = 0; i < count; i++) {
		*stream >> id;
		if (nodes->contains(id)) {
			nodesToUnRestrict->append(*nodes->find(id));
		}
	}


	cw->setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> (NULL),
				currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler> (NULL),
				nodesToUnRestrict
				);
	Server * server = Server::getInstance();
	server->sendUnSetRestriction(nodesToUnRestrict);
}
