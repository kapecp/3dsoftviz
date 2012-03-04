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

        client->addClient(id,nick);
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

     client->updateUserList();
     client->sendMyView();
}
