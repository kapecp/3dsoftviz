#include "Network/executors/MoveNodeExecutor.h"
#include "Manager/Manager.h"

using namespace Network;

void MoveNodeExecutor::execute() {

    int id;
    float x,y,z;

    *stream >> id >> x >> y >> z;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    //qDebug() << "Moving" << id << "to" << x << y << z;
    Data::Node *node = (*((*nodes).find(id)));

    node -> setUsingInterpolation(false);
    node -> setFixed(true);
    node -> setTargetPosition(osg::Vec3(x,y,z));
}
