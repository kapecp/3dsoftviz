#include "Network/executors/MoveAvatarExecutor.h"
#include "Network/Client.h"

using namespace Network;

MoveAvatarExecutor::MoveAvatarExecutor(QRegExp regex){
    this->regexp = regex;
}

void MoveAvatarExecutor::execute() {

    Client *client = Client::getInstance();

    osg::Vec3d center = osg::Vec3d(regexp.cap(1).toFloat()-5,regexp.cap(2).toFloat(),regexp.cap(3).toFloat());
    osg::Quat rotation = osg::Quat(regexp.cap(4).toFloat(),regexp.cap(5).toFloat(),regexp.cap(6).toFloat(),regexp.cap(7).toFloat());
    int id = regexp.cap(8).toInt();

    if (client->userToSpy() != id) {
        osg::PositionAttitudeTransform * PAtransform = client->avatarList[id];
        if (PAtransform != NULL) {
            PAtransform->setAttitude(rotation);
            PAtransform->setPosition(center);
        } else {
            qDebug() << "Nepoznam avatar" << id;
        }
    }

    if (client->userToSpy() == id) {
        client->setMyView(center,rotation);
    }

}
