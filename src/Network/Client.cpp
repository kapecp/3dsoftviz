/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"
#include "Importer/GraphOperations.h"
#include "Manager/Manager.h"
#include "QOSG/CoreWindow.h"

#include "Network/ExecutorFactory.h"

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
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        //qDebug() << "Client got line: " << line;


        //currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

        ExecutorFactory *executorFactory = new ExecutorFactory();
        AbstractExecutor *executor = executorFactory->getExecutor(line);

        if (executor != NULL) {
            executor->execute();
        } else {
            qDebug() << "EXECUTOR: neznama instrukcia";
        }



        QRegExp messageRegex("^([^:]+):(.*)$");

        QRegExp layRegexp("^/layData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

        QRegExp viewRegexp("^/view:center:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);rotation:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);id:([0-9]+)$");

        if (layRegexp.indexIn(line) != -1) {
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
            int id = viewRegexp.cap(8).toInt();
            osg::PositionAttitudeTransform * PAtransform = avatarList[id];
            if (PAtransform != NULL) {
                PAtransform->setAttitude(rotation);
                PAtransform->setPosition(center);
            } else {
                qDebug() << "Nepoznam avatar" << id;
            }
        } else if(messageRegex.indexIn(line) != -1) {
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);

            qDebug() << user + ": " + message;

        } else if (line == "SERVER_STOP") {
            this->disconnect();
        } else if (line == "WELCOME") {

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

void Client::sendMyView(osg::Vec3d center, osg::Quat rotation) {

    QString message = "/view:";
    message += "center:" + QString::number(center.x()) + "," + QString::number(center.y()) + "," + QString::number(center.z()) + ";";
    message += "rotation:" + QString::number(rotation.x()) + "," + QString::number(rotation.y()) + "," + QString::number(rotation.z()) + "," + QString::number(rotation.w()) +  "\n";

    socket->write(message.toUtf8());
}
