#include "Network/executors/LayoutExecutor.h"
#include "Network/Client.h"

using namespace Network;

LayoutExecutor::LayoutExecutor(QRegExp regex){
    this->regexp = regex;
}

void LayoutExecutor::execute() {

    Client *client = Client::getInstance();

    int id = regexp.cap(1).toInt();

    float x = regexp.cap(2).toFloat();
    float y = regexp.cap(3).toFloat();
    float z = regexp.cap(4).toFloat();

    client->thread->pause();

    if (!client->selected_nodes.contains(client->nodes[id])){
        client->nodes[id]->setTargetPosition(osg::Vec3(x,y,z));
    }

    //qDebug()<< "[NEW NODE POS] id: " << id << " [" << x << "," << y << "," << z << "]";
}
