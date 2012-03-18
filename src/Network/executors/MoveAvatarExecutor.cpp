#include "Network/executors/MoveAvatarExecutor.h"
#include "Network/Client.h"

using namespace Network;

void MoveAvatarExecutor::execute() {

    Client *client = Client::getInstance();

    float x,y,z,a,b,c,d;
    int id;

    *stream >> x >> y >> z >> a >> b >> c >> d >> id;

    osg::Vec3d center = osg::Vec3d(x-5,y,z);
    osg::Quat rotation = osg::Quat(a,b,c,d);

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

    if (client->getCenterUser() == id) {
        client->lookAt(center);
    }

}
