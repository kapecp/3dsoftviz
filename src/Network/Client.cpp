/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"
#include "Importer/GraphOperations.h"
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
    user_to_spy = -1;

    executorFactory = new ExecutorFactory(this);

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

void Client::readyRead() {
    while(socket->canReadLine())
    {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        //qDebug() << "Client got line: " << line;

        AbstractExecutor *executor = executorFactory->getExecutor(line);

        if (executor != NULL) {
            executor->execute();
        } else {
            qDebug() << "Client: neznama instrukcia:" << line;
        }

    }
}

void Client::connected() {
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

        socket->write(QString("/SMoveNode:" + message + "\n").toUtf8());
    }

    selected_nodes.clear();
}

void Client::sendMyView(osg::Vec3d center, osg::Quat rotation) {

    QString message = "/view:";
    message += "center:" + QString::number(center.x()) + "," + QString::number(center.y()) + "," + QString::number(center.z()) + ";";
    message += "rotation:" + QString::number(rotation.x()) + "," + QString::number(rotation.y()) + "," + QString::number(rotation.z()) + "," + QString::number(rotation.w()) +  "\n";

    socket->write(message.toUtf8());
}

void Client::updateUserList() {
    QListWidgetItem * item;
    QListWidget * lw = ((QOSG::CoreWindow *) cw)->lw_users;
    lw->clear();
    QMap<int,QString>::iterator i = userList.begin();
    while (i != userList.end()) {
        item = new QListWidgetItem();
        item->setData(6,i.key());
        item->setText(i.value());
        lw->addItem(item);
        i++;
    }
}

void Client::setMyView(osg::Vec3d center, osg::Quat rotation) {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    cameraManipulator->setCenter(center);
    cameraManipulator->setRotation(rotation);
}

void Client::addClient(int id, QString nick) {
    if (!userList.contains(id)){
        userList.insert(id,nick);
        addAvatar(id);
    }
}

void Client::addAvatar(int id) {
    osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("avatar.osg");
    if (!modelNode) {
        qDebug() << "could not find model";
        return;
    }

    osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
    PAtransform->addChild(modelNode);

    QLinkedList<osg::ref_ptr<osg::Node> > * nodes = coreGraph->getCustomNodeList();

    nodes->append(PAtransform);

    //PAtransform->setScale(osg::Vec3d(10,10,10));
    avatarList.insert(id,PAtransform);
}

void Client::hideClientAvatar(int id) {
    osg::PositionAttitudeTransform *pat = avatarList.take(id);
    if (pat != NULL) {
        pat->removeChild(0,1);
    }
}

void Client::showClientAvatar(int id) {
    addAvatar(id);
}

void Client::unSpyUser() {
    showClientAvatar(user_to_spy);
    QString message = "/unspy:"+QString::number(user_to_spy)+"\n";
    socket->write(message.toUtf8());
    user_to_spy = -1;
}

void Client::spyUser(int user) {
    QString message = "/spying:"+QString::number(user)+"\n";
    qDebug() << "posielam" << message;
    socket->write(message.toUtf8());
    user_to_spy = user;
    hideClientAvatar(user);
}
