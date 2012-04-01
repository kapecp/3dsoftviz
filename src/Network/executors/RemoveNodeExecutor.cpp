#include "Network/executors/RemoveNodeExecutor.h"
#include "Manager/Manager.h"

using namespace Network;

void RemoveNodeExecutor::execute() {

    int id;

    *stream >> id;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    //qDebug() << "Moving" << id << "to" << x << y << z;
    Data::Node *node = (*((*nodes).find(id)));
    currentGraph->removeNode(node);

}
