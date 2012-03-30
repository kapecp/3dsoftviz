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

    float color_x, color_y, color_z, color_w;

    QString name;

    *stream >> id >> x >> y >> z >> name >> color_x >> color_y >> color_z >> color_w;
    //qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

    osg::Vec3 position(x,y,z);
    osg::ref_ptr<Data::Node> newNode  = client->currentGraph->addNode(id,name, client->nodeType, position);

    newNode->setColor(osg::Vec4(color_x, color_y, color_z, color_w));
}
