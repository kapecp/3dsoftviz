#include "Network/executors/LayoutExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

void LayoutExecutor::execute() {

    Client *client = Client::getInstance();

    int id;

    float x,y,z;

    *stream >> id >> x >> y >> z;

    client->thread->pause();

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    osg::ref_ptr<Data::Node> node = (*((*nodes).find(id)));

    if (!client->selected_nodes.contains(node)){
        node->setTargetPosition(osg::Vec3(x,y,z));
    }

    //qDebug()<< "[NEW NODE POS] id: " << id << " [" << x << "," << y << "," << z << "]";
}
