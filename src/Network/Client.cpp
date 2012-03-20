/**
 Client - server communication implementation inspired by http://thesmithfam.org/blog/2009/07/09/example-qt-chat-program/
 */

#include "Network/Client.h"
#include "Network/Helper.h"
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
    user_to_center = -1;
    layoutIgnore = true;

    executorFactory = new ExecutorFactory();

    blockSize = 0;

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
    // todo: implement
}

void Client::requestGraph() {
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    out << (quint16)0 << ServerSendGraphExecutor::INSTRUCTION_NUMBER;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);
}

void Client::readyRead() {


    QTcpSocket *senderClient = (QTcpSocket*)sender();

    while(senderClient->bytesAvailable()) {

        QDataStream in(senderClient);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);

        if (blockSize == 0) {
            if (senderClient->bytesAvailable() < (int)sizeof(quint16))
                return;

            in >> blockSize;
        }

        if (senderClient->bytesAvailable() < blockSize)
            return;

        AbstractExecutor *executor = executorFactory->getExecutor(&in);

        if (executor != NULL) {
            executor->execute();
        } else {
            qDebug() << "Klient: neznama instrukcia";
        }
        blockSize = 0;
    }
}

void Client::connected() {

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out << (quint16)0;
    out << ServerIncommingUserExecutor::INSTRUCTION_NUMBER;
    out << clientNick;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);

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

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    while (i != selected_nodes.constEnd())
    {

        block.clear();
        out.device()->reset();

        out << (quint16)0 << ServerMoveNodeExecutor::INSTRUCTION_NUMBER
            << (int) ((*i)->getId())
            << (float) ((*i)->getCurrentPosition().x())
            << (float) ((*i)->getCurrentPosition().y())
            << (float) ((*i)->getCurrentPosition().z());
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));

        ++i;

        socket->write(block);
    }

    selected_nodes.clear();
}

void Client::sendMyView(osg::Vec3d center, osg::Quat rotation, float distance) {

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);

    out << (quint16)0 << ServerMoveAvatarExecutor::INSTRUCTION_NUMBER << (float)center.x() << (float)center.y() << (float)center.z()
        << (float)rotation.x() << (float)rotation.y() << (float)rotation.z() << (float)rotation.w() << (float) distance;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);
}

void Client::sendMyView() {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    this->sendMyView(cameraManipulator->getCenter(), cameraManipulator->getRotation(), cameraManipulator->getDistance());
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

void Client::setMyView(osg::Vec3d center, osg::Quat rotation, float distance) {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    cameraManipulator->setCenter(center);
    cameraManipulator->setRotation(rotation);
    cameraManipulator->setDistance(distance);
}

void Client::addClient(int id, QString nick) {
    if (!userList.contains(id)){
        userList.insert(id,nick);
        addAvatar(id,nick);
    }
}

void Client::addAvatar(int id, QString nick) {

    osg::PositionAttitudeTransform* PAtransform = Helper::generateAvatar(nick);

    QLinkedList<osg::ref_ptr<osg::Node> > * nodes = coreGraph->getCustomNodeList();

    nodes->append(PAtransform);

    //PAtransform->setScale(osg::Vec3d(10,10,10));
    avatarList.insert(id,PAtransform);
}

void Client::removeAvatar(int id) {
    osg::PositionAttitudeTransform *pat = avatarList.take(id);
    if (pat != NULL) {
        pat->removeChild(0,2);
    }
}

void Client::showClientAvatar(int id) {
    addAvatar(id, userList[id]);
}

void Client::unSpyUser() {
    showClientAvatar(user_to_spy);

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    out << (quint16)0 << ServerUnspyUserExecutor::INSTRUCTION_NUMBER << (int)user_to_spy;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);

    // restore original view
    setMyView(original_center,original_rotation,original_distance);

    sendMyView();
    user_to_spy = -1;
}

void Client::spyUser(int user) {

    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    out << (quint16)0 << ServerSpyUserExecutor::INSTRUCTION_NUMBER << user;

    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    socket->write(block);
    user_to_spy = user;

    // store original view
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    original_center = cameraManipulator->getCenter();
    original_rotation = cameraManipulator->getRotation();
    original_distance = cameraManipulator->getDistance();

    setMyView(avatarList[user]->getPosition(),avatarList[user]->getAttitude(),original_distance);
    removeAvatar(user);
}

void Client::lookAt(osg::Vec3d coord) {
    Vwr::CameraManipulator * cameraManipulator = ((QOSG::CoreWindow *) cw)->getCameraManipulator();
    osg::Quat rotation = Helper::lookAt(cameraManipulator->getCenter(), coord);
    cameraManipulator->setRotation(rotation);
    sendMyView();
}

void Client::centerUser(int id_user) {
    user_to_center = id_user;
    osg::PositionAttitudeTransform * userAvatar = avatarList[id_user];
    lookAt(userAvatar->getPosition());
}
