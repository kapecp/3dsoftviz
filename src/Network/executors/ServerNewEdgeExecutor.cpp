#include "Network/executors/ServerNewEdgeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

using namespace Network;

void ServerNewEdgeExecutor::execute() {

    Server * server = Server::getInstance();

    QString name;
    int from, to;
    bool oriented;

    *stream >> name >> from >> to >> oriented;
    //qDebug()<< "[NEW NODE]" << "[" << x << "," << y << "," << z << "]";


    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();

    osg::ref_ptr<Data::Node> node_from = (*((*nodes).find(from)));
    osg::ref_ptr<Data::Node> node_to = (*((*nodes).find(to)));

    Data::Type* type = currentGraph->addType(Data::GraphLayout::META_EDGE_TYPE);

    osg::ref_ptr<Data::Edge> newEdge = currentGraph->addEdge("NewEdge",node_from,node_to,type,oriented);

    if (((QOSG::CoreWindow *)server->getCoreWindowReference())->playing()) {
        server->getLayoutThread()->play();
    }

    server->sendNewEdge(newEdge);
}
