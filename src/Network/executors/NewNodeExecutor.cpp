#include "Network/executors/NewNodeExecutor.h"
#include "Network/Client.h"
#include "osg/Vec3"
#include "osg/ref_ptr"
#include "Data/Node.h"

using namespace Network;

void NewNodeExecutor::execute() {

    Client * client = Client::getInstance();

    int id;

    float x;
    float y;
    float z;

    QString name;

    *stream >> id >> x >> y >> z >> name;
    //qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

    osg::Vec3 position(x,y,z);
    client->currentGraph->addNode(id,name, client->nodeType, position);
}
