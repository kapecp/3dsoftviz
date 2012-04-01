#include "Network/executors/MoveNodeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

using namespace Network;

void MoveNodeExecutor::execute_client() {

    int id;
    float x,y,z;

    *stream >> id >> x >> y >> z;

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    //qDebug() << "Moving" << id << "to" << x << y << z;
    Data::Node *node = (*((*nodes).find(id)));
    if (Client::getInstance()->selected_nodes.contains(node)) {
        return;
    }

    node -> setUsingInterpolation(false);
    node -> setFixed(true);
    node -> setTargetPosition(osg::Vec3(x,y,z));
}

void MoveNodeExecutor::execute_server() {

    int id;
    float x,y,z;

    Server * server = Server::getInstance();
    float graphScale = server->getGraphScale();
    QSet<QTcpSocket*> clients = server->getClients();

    *stream >> id >> x >> y >> z;

    x/=graphScale;
    y/=graphScale;
    z/=graphScale;

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    out << (quint16)0 << MoveNodeExecutor::INSTRUCTION_NUMBER << id << x << y << z;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    foreach(QTcpSocket *otherClient, clients) {
        otherClient->write(block);
    }

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
    //qDebug() << "Moving" << id << "to" << x << y << z;
    Data::Node *node = (*((*nodes).find(id)));

    server->appendMovingNode(node);

    node -> setUsingInterpolation(false);
    node -> setFixed(true);
    node -> setTargetPosition(osg::Vec3(x,y,z));
    Layout::LayoutThread * thread = server->getLayoutThread();
    thread->wakeUp();

}
