#include "Network/executors/MessageExecutor.h"
#include <QDebug>

using namespace Network;

void MessageExecutor::execute_server() {

    QString user, message;
    *stream >> user >> message;

    qDebug() << user + ": " + message;
}

void MessageExecutor::execute_client() {
    return;
}
