#include "Network/executors/SetFixNodeStateExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

void SetFixNodeStateExecutor::execute_client() {
    int id;
    bool state;

    *stream >> id >> state;

    Client * client = Client::getInstance();

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    if (nodes->contains(id)) {
        Data::Node *node = *nodes->find(id);
        node->setFixed(state);
        node->setUsingInterpolation(!state);
        if (client->selected_nodes.contains(node)) {
            client->selected_nodes.removeOne(node);
        }
    }

}

void SetFixNodeStateExecutor::execute_server() {
    int id;
    bool state;

    *stream >> id >> state;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    if (nodes->contains(id)) {
        Data::Node *node = *nodes->find(id);
        node->setFixed(state);
        node->setUsingInterpolation(!state);
    }

    Server * server = Server::getInstance();
    server->sendFixNodeState(id, state);

    if (((QOSG::CoreWindow *)server->getCoreWindowReference())->playing()) {
        server->getLayoutThread()->play();
    }

}
