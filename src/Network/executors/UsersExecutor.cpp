#include <QStringList>

#include "Network/executors/UsersExecutor.h"
#include "Network/Client.h"


using namespace Network;

UsersExecutor::UsersExecutor(QRegExp regex){
    this->regexp = regex;
}

void UsersExecutor::execute() {

    Client *client = Client::getInstance();

    QStringList users = regexp.cap(1).split(",");
    qDebug() << "Clients:";

    QList<int> newClients;

    foreach(QString user, users){
        QStringList args = user.split("=");
        int id = args[0].toInt();
        QString nick = args[1];
        qDebug() << id << nick;
        newClients << id;

        if (!client->userList.contains(id)){
            client->userList.insert(id,nick);

            osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("avatar.osg");
            if (!modelNode) {
                qDebug() << "could not find model";
                return;
            }

            osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
            PAtransform->addChild(modelNode);

            QLinkedList<osg::ref_ptr<osg::Node> > * nodes = client->coreGraph->getCustomNodeList();

            nodes->append(PAtransform);

            //PAtransform->setScale(osg::Vec3d(10,10,10));
            client->avatarList.insert(id,PAtransform);
        }
    }

    //delete disconnected users
    QMap<int, QString>::iterator i = client->userList.begin();
     while (i != client->userList.end()) {
         if ( (!newClients.contains(i.key()) && i.key() != 0)){
             client->avatarList[i.key()]->removeChild(0,1);
             client->avatarList.remove(i.key());
             i = client->userList.erase(i);
         } else {
             ++i;
         }
     }
}
