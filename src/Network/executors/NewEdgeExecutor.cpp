#include "Network/executors/NewEdgeExecutor.h"
#include "Network/Client.h"

using namespace Network;

NewEdgeExecutor::NewEdgeExecutor(QRegExp regex){
    this->regexp = regex;
}

void NewEdgeExecutor::execute() {

    Client * client = Client::getInstance();

    int id = regexp.cap(1).toInt();
    int from = regexp.cap(2).toInt();
    int to = regexp.cap(3).toInt();
    bool oriented = regexp.cap(4).toInt();

    //qDebug()<< "[NEW EDGE] id: " << id << " from: " << from << ", to:" << to;

    client->currentGraph->addEdge(id,"NewEdge",client->nodes[from],client->nodes[to],client->edgeType,oriented);
}
