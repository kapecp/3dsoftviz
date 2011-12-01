#include "Network/executors/NewNodeExecutor.h"
#include "Network/Client.h"
#include "osg/Vec3"
#include "osg/ref_ptr"
#include "Data/Node.h"

using namespace Network;

NewNodeExecutor::NewNodeExecutor(QRegExp regex){
    this->regexp = regex;
}

void NewNodeExecutor::execute() {

    Client * client = Client::getInstance();

    int id = regexp.cap(1).toInt();

    float x = regexp.cap(2).toFloat();
    float y = regexp.cap(3).toFloat();
    float z = regexp.cap(4).toFloat();

    //qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

    osg::Vec3 position(x,y,z);
    osg::ref_ptr<Data::Node> node = client->currentGraph->addNode(id,"newNode", client->nodeType, position);
    client->nodes[id] = node;
}
