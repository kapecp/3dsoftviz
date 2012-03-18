#include "Network/executors/MessageExecutor.h"
#include <QDebug>

using namespace Network;

void MessageExecutor::execute() {

    QString user, message;
    *stream >> user >> message;

    qDebug() << user + ": " + message;
}
