/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"
#include "Importer/GraphOperations.h"
#include <QRegExp>
#include "Manager/Manager.h"
#include "QOSG/CoreWindow.h"

using namespace Network;

Client * Client::instance;

Client::Client(QObject *parent) : QObject(parent) {

    instance = this;

    socket = new QTcpSocket(this);

    cw = parent;

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));

    edgeType = NULL;
    nodeType = NULL;

}

Client* Client::getInstance() {
    if (instance == NULL) {
        return new Client();
    } else {
        return instance;
    }
}

void Client::ServerConnect(QString nick, QString address) {
    clientNick = nick;
    socket->connectToHost(address, 4200);
}

void Client::send_message(QString message) {
    if(!message.isEmpty())
    {
        socket->write(QString(message + "\n").toUtf8());
    }
}

// This function gets called whenever the server has sent us some text:
void Client::readyRead() {
    // We'll loop over every (complete) line of text that the server has sent us:
    while(socket->canReadLine())
    {
        QTime t;
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        //qDebug() << "Client got line: " << line;

        QRegExp messageRegex("^([^:]+):(.*)$");

        QRegExp usersRegex("^/clients:(.*)$");

        QRegExp nodeRegexp("^/nodeData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

        QRegExp layRegexp("^/layData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

        QRegExp edgeRegexp("^/edgeData:id:([0-9]+);from:([0-9]+);to:([0-9]+);or:([01])$");

        QRegExp moveNodeRegexp("^/moveNode:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

        QRegExp viewRegexp("^/view:center:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);rotation:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+)$");

        Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

        if(usersRegex.indexIn(line) != -1)
        {
            QStringList users = usersRegex.cap(1).split(",");
            qDebug() << "Clients:";
            foreach(QString user, users){
                QStringList args = user.split("=");
                int id = args[0].toInt();
                QString nick = args[1];

                qDebug() << id << nick;

                if (!userList.contains(id)){
                    userList.insert(id,nick);

                    osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("kocka.osg");
                    if (!modelNode) {
                        qDebug() << "could not find model";
                        return;
                    }

                    osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
                    PAtransform->addChild(modelNode);

                    QLinkedList<osg::ref_ptr<osg::Node> > * nodes = coreGraph->getCustomNodeList();

                    nodes->append(PAtransform);

                    PAtransform->setScale(osg::Vec3d(10,10,10));
                    avatarList.insert(id,PAtransform);
                }
            }

        } else if (moveNodeRegexp.indexIn(line) != -1) {
            int id = moveNodeRegexp.cap(1).toInt();

            float x = moveNodeRegexp.cap(2).toFloat();
            float y = moveNodeRegexp.cap(3).toFloat();
            float z = moveNodeRegexp.cap(4).toFloat();
            Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
            QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes = currentGraph -> getNodes();
            //qDebug() << "Moving" << id << "to" << x << y << z;
            Data::Node *node = (*((*nodes).find(id)));

            node -> setUsingInterpolation(false);
            node -> setFixed(true);
            node -> setTargetPosition(osg::Vec3(x,y,z));
        } else if (line == "GRAPH_START") {
            t.start();
            currentGraph= Manager::GraphManager::getInstance()->createNewGraph("NewGraph");
            Importer::GraphOperations * operations = new Importer::GraphOperations(*currentGraph);
            operations->addDefaultTypes(edgeType, nodeType);

            thread->pause();
            coreGraph->setNodesFreezed(true);
            currentGraph->setFrozen(true);
        } else if (line == "GRAPH_END") {
            qDebug() << "Building took" << QString::number(t.elapsed()) << "ms";
            thread->play();
            coreGraph->setNodesFreezed(false);
            currentGraph->setFrozen(false);
        } else if (nodeRegexp.indexIn(line) != -1){
            int id = nodeRegexp.cap(1).toInt();

            float x = nodeRegexp.cap(2).toFloat();
            float y = nodeRegexp.cap(3).toFloat();
            float z = nodeRegexp.cap(4).toFloat();

            //qDebug()<< "[NEW NODE] id: " << id << " [" << x << "," << y << "," << z << "]";

            osg::Vec3 position(x,y,z);
            osg::ref_ptr<Data::Node> node = currentGraph->addNode(id,"newNode", nodeType, position);
            nodes[id] = node;

        } else if (edgeRegexp.indexIn(line) != -1) {
            int id = edgeRegexp.cap(1).toInt();
            int from = edgeRegexp.cap(2).toInt();
            int to = edgeRegexp.cap(3).toInt();
            bool oriented = edgeRegexp.cap(4).toInt();

            //qDebug()<< "[NEW EDGE] id: " << id << " from: " << from << ", to:" << to;

            currentGraph->addEdge(id,"NewEdge",nodes[from],nodes[to],edgeType,oriented);
        } else if (layRegexp.indexIn(line) != -1) {
            int id = layRegexp.cap(1).toInt();

            float x = layRegexp.cap(2).toFloat();
            float y = layRegexp.cap(3).toFloat();
            float z = layRegexp.cap(4).toFloat();

            thread->pause();

            if (!selected_nodes.contains(nodes[id])){
                nodes[id]->setTargetPosition(osg::Vec3(x,y,z));
            }

            //qDebug()<< "[NEW NODE POS] id: " << id << " [" << x << "," << y << "," << z << "]";

        } else if (viewRegexp.indexIn(line) != -1) {
            osg::Vec3d center = osg::Vec3d(viewRegexp.cap(1).toFloat()-5,viewRegexp.cap(2).toFloat(),viewRegexp.cap(3).toFloat());
            osg::Quat rotation = osg::Quat(viewRegexp.cap(4).toFloat(),viewRegexp.cap(5).toFloat(),viewRegexp.cap(6).toFloat(),viewRegexp.cap(7).toFloat());
            /*if (PAtransform != NULL) {
                PAtransform->setAttitude(rotation);
                PAtransform->setPosition(center);
            }*/
        } else if(messageRegex.indexIn(line) != -1) {
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            qDebug() << user + ": " + message;

        } else if (line == "SERVER_STOP") {
            this->disconnect();
        } else if (line == "WELCOME") {
            int id = 0;
            QString nick = "server";

            userList.insert(id,nick);

            osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("kocka.osg");
            if (!modelNode) {
                qDebug() << "could not find model";
                return;
            }

            osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
            PAtransform->addChild(modelNode);

            QLinkedList<osg::ref_ptr<osg::Node> > * nodes = coreGraph->getCustomNodeList();

            nodes->append(PAtransform);

            PAtransform->setScale(osg::Vec3d(10,10,10));
            avatarList.insert(id,PAtransform);
        }
    }
}

void Client::connected()
{
    socket->write(QString("/me:" + clientNick + "\n").toUtf8());
    ((QOSG::CoreWindow *) cw) -> le_client_name -> setEnabled(false);
    ((QOSG::CoreWindow *) cw) -> le_server_addr -> setEnabled(false);
    ((QOSG::CoreWindow *) cw) -> b_start_client -> setEnabled(true);
    ((QOSG::CoreWindow *) cw) -> b_start_client -> setText("Disconnect");
}

void Client::error(){
    this->disconnect();
}

void Client::disconnected(){
    this->disconnect();
}

void Client::disconnect() {
    if (socket -> isOpen()){
        socket -> disconnectFromHost();
        socket -> close();
    }
    ((QOSG::CoreWindow *) cw) -> le_client_name -> setEnabled(true);
    ((QOSG::CoreWindow *) cw) -> le_server_addr -> setEnabled(true);
    ((QOSG::CoreWindow *) cw) -> b_start_client -> setEnabled(true);
    ((QOSG::CoreWindow *) cw) -> b_start_client -> setText("Connect to session");
    ((QOSG::CoreWindow *) cw) -> b_start_server -> setEnabled(true);
}

void Client::setLayoutThread(Layout::LayoutThread *layoutThread){
    thread = layoutThread;
}

bool Client::isConnected() {
    return socket -> state() == QAbstractSocket::ConnectedState;
}

void Client::sendMovedNodesPosition() {
    QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selected_nodes.constBegin();
    QString message;

    while (i != selected_nodes.constEnd())
    {

        message = "id:" + QString::number((*i)->getId());
        message += ";x:" + QString::number((*i)->getCurrentPosition().x());
        message += ";y:" + QString::number((*i)->getCurrentPosition().y());
        message += ";z:" + QString::number((*i)->getCurrentPosition().z());
        ++i;

        socket->write(QString("/moveNode:" + message + "\n").toUtf8());
    }

    selected_nodes.clear();
}
