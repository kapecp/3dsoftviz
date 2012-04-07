#include "Network/executors/SetRestrictionExecutor.h"
#include "Manager/Manager.h"
#include "Importer/GraphOperations.h"
#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"
#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"

using namespace Network;

void SetRestrictionExecutor::execute_client() {
    int id;
    float x,y,z;
    QString name;

    *stream >> id >> name >> x >> y >> z;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (name, osg::Vec3f(x,y,z));

    *stream >> id >> name >> x >> y >> z;
    osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (name, osg::Vec3f(x,y,z));

    Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
    restrictionNodes.push_back (centerNode);
    restrictionNodes.push_back (surfaceNode);

    Client * client = Client::getInstance();
    QOSG::CoreWindow * cw = (QOSG::CoreWindow *) client->getCoreWindowReference();

    QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict = new QLinkedList<osg::ref_ptr<Data::Node> >();

    int count;

    *stream >> count;

    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph->getNodes();
    for (int i = 0; i < count; i++) {
        *stream >> id;
        if (nodes->contains(id)) {
            nodesToRestrict->append(*nodes->find(id));
        }
    }

    cw->setRestrictionToSelectedNodes(
                QSharedPointer<Layout::ShapeGetter> (
                    new Layout::ShapeGetter_SphereSurface_ByTwoNodes (centerNode, surfaceNode)
                    ),
                currentGraph,
                QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
                    new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
                        *currentGraph,
                        restrictionNodes
                        )
                    ),
                nodesToRestrict
                );

}

void SetRestrictionExecutor::execute_server() {

}
