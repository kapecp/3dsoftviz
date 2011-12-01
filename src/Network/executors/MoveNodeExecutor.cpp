#include "Network/executors/MoveNodeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Client.h"

using namespace Network;

MoveNodeExecutor::MoveNodeExecutor(QRegExp regex){
    this->regexp = regex;
}

void MoveNodeExecutor::execute() {
    int id = regexp.cap(1).toInt();

    float x = regexp.cap(2).toFloat();
    float y = regexp.cap(3).toFloat();
    float z = regexp.cap(4).toFloat();
    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    //qDebug() << "Moving" << id << "to" << x << y << z;
    Data::Node *node = (*((*nodes).find(id)));

    node -> setUsingInterpolation(false);
    node -> setFixed(true);
    node -> setTargetPosition(osg::Vec3(x,y,z));
}
