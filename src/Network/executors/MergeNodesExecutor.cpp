#include "Network/executors/MergeNodesExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

void MergeNodesExecutor::execute_client() {

    int count, id;

    *stream >>count;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();

    QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = new QLinkedList<osg::ref_ptr<Data::Node> >();

    for (int i = 0; i < count; i++) {
        *stream >> id;
        if (nodes->contains(id)) {
            selectedNodes->append(*nodes->find(id));
        }
    }

    *stream >> id;

    float x, y, z;
    *stream >> x >> y >> z;
    osg::Vec3 position = osg::Vec3(x, y, z);

    currentGraph->mergeNodes(selectedNodes, position, id);

}

void MergeNodesExecutor::execute_server() {

    int count, id;

    *stream >>count;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();

    QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = new QLinkedList<osg::ref_ptr<Data::Node> >();

    for (int i = 0; i < count; i++) {
        *stream >> id;
        if (nodes->contains(id)) {
            selectedNodes->append(*nodes->find(id));
        }
    }

    float x, y, z;
    *stream >> x >> y >> z;
    osg::Vec3 position = osg::Vec3(x, y, z);

    Server * server = Server::getInstance();
    osg::ref_ptr<Data::Node> mergeNode = currentGraph->mergeNodes(selectedNodes, position);
    server->sendMergeNodes(selectedNodes, position, mergeNode->getId());

    if (((QOSG::CoreWindow *)server->getCoreWindowReference())->playing()) {
        server->getLayoutThread()->play();
    }
    return;

}
