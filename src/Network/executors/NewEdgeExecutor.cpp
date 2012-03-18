#include "Network/executors/NewEdgeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

void NewEdgeExecutor::execute() {

    Client * client = Client::getInstance();

    int id;
    int from;
    int to;
    bool oriented;

    *stream >> id >> from >> to >> oriented;

    //qDebug()<< "[NEW EDGE] id: " << id << " from: " << from << ", to:" << to;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();

    osg::ref_ptr<Data::Node> node_from = (*((*nodes).find(from)));
    osg::ref_ptr<Data::Node> node_to = (*((*nodes).find(to)));

    client->currentGraph->addEdge(id,"NewEdge",node_from,node_to,client->edgeType,oriented);
}
