#include "Network/executors/ServerMoveNodeExecutor.h"
#include "Manager/Manager.h"
#include "Network/Server.h"

using namespace Network;

ServerMoveNodeExecutor::ServerMoveNodeExecutor(QRegExp regex){
    this->regexp = regex;
}

void ServerMoveNodeExecutor::execute() {

    int id = regexp.cap(1).toInt();

    Server * server = Server::getInstance();
    float graphScale = server->getGraphScale();
    QSet<QTcpSocket*> clients = server->getClients();

    float x = regexp.cap(2).toFloat()/graphScale;
    float y = regexp.cap(3).toFloat()/graphScale;
    float z = regexp.cap(4).toFloat()/graphScale;

    QString message = "/moveNode:id:"+ QString::number(id) + ";x:" + QString::number(x) + ";y:" + QString::number(y) + ";z:" + QString::number(z) + "\n";
    foreach(QTcpSocket *otherClient, clients) {
        otherClient->write(message.toUtf8());
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
    thread->play();

}
