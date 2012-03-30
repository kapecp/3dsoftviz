#include "Network/executors/ServerNewNodeExecutor.h"
#include "Manager/Manager.h"
#include "Importer/GraphOperations.h"
#include "Network/Server.h"
#include "osg/Vec3"
#include "osg/ref_ptr"
#include "Data/Node.h"

using namespace Network;

void ServerNewNodeExecutor::execute() {

    Server * server = Server::getInstance();
    QTcpSocket * sender = (QTcpSocket*) stream->device();
    int id;

    float x;
    float y;
    float z;

    float color_x, color_y, color_z, color_w;

    QString name;

    *stream >> id >> x >> y >> z >> name >> color_x >> color_y >> color_z >> color_w;
    //qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

    osg::Vec3 position(x/server->getGraphScale(),y/server->getGraphScale(),z/server->getGraphScale());
    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    if (currentGraph == NULL) {
        currentGraph = Manager::GraphManager::getInstance()->createNewGraph("New graph");
    }

    Data::Type *nodeType;
    Data::Type *edgeType;
    Importer::GraphOperations * operations = new Importer::GraphOperations(*currentGraph);
    operations->addDefaultTypes(edgeType, nodeType);

    osg::ref_ptr<Data::Node> newNode  = currentGraph->addNode(id,name, nodeType, position);
    newNode->setColor(osg::Vec4(color_x, color_y, color_z, color_w));

    QSet<QTcpSocket*> clients = server->getClients();
    foreach(QTcpSocket *client, clients) { // send new node to others
        if (client == sender) continue;
        server->sendNewNode(newNode, client);
    }
}
