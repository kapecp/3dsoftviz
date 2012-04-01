#include "Network/executors/ServerRemoveNodeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

using namespace Network;

void ServerRemoveNodeExecutor::execute() {

    int id;

    *stream >> id;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    //qDebug() << "Moving" << id << "to" << x << y << z;
    Data::Node *node = (*((*nodes).find(id)));
    currentGraph->removeNode(node);

    Server * server = Server::getInstance();
    server->sendRemoveNode(id);

    if (((QOSG::CoreWindow *)server->getCoreWindowReference())->playing()) {
        server->getLayoutThread()->play();
    }

}
