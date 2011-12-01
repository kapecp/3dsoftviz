#include "Network/executors/LayoutExecutor.h"
#include "Manager/Manager.h"
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

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    osg::ref_ptr<Data::Node> node = (*((*nodes).find(id)));

    if (!client->selected_nodes.contains(node)){
        node->setTargetPosition(osg::Vec3(x,y,z));
    }

    //qDebug()<< "[NEW NODE POS] id: " << id << " [" << x << "," << y << "," << z << "]";
}
