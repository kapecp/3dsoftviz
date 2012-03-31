#include "Network/executors/ServerNewNodeExecutor.h"
#include "Manager/Manager.h"
#include "Importer/GraphOperations.h"
#include "Network/Server.h"

using namespace Network;

void ServerNewNodeExecutor::execute() {

    Server * server = Server::getInstance();

    float x;
    float y;
    float z;

    QString name;

    *stream >> x >> y >> z >> name;
    //qDebug()<< "[NEW NODE]" << "[" << x << "," << y << "," << z << "]";

    osg::Vec3 position(x/server->getGraphScale(),y/server->getGraphScale(),z/server->getGraphScale());
    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    if (currentGraph == NULL) {
        currentGraph = Manager::GraphManager::getInstance()->createNewGraph("New graph");
    }

    Data::Type *nodeType;
    Data::Type *edgeType;
    Importer::GraphOperations * operations = new Importer::GraphOperations(*currentGraph);
    operations->addDefaultTypes(edgeType, nodeType);

    osg::ref_ptr<Data::Node> newNode  = currentGraph->addNode(name, nodeType, position);

    if (((QOSG::CoreWindow *)server->getCoreWindowReference())->playing()) {
        server->getLayoutThread()->play();
    }

    server->sendNewNode(newNode);
}
