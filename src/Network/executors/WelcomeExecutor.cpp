#include "Network/executors/WelcomeExecutor.h"
#include "Network/Client.h"

using namespace Network;

WelcomeExecutor::WelcomeExecutor(QRegExp regex) {
    this->regexp = regex;
}

void WelcomeExecutor::execute() {
    Client * client = Client::getInstance();
    int id = regexp.cap(1).toInt();
    client->setMyId(id);

    qDebug() << "Som " << id;

}
