#include "Network/executors/MessageExecutor.h"
#include <QDebug>

using namespace Network;

MessageExecutor::MessageExecutor(QRegExp regex){
    this->regexp = regex;
}

void MessageExecutor::execute() {

    QString user = regexp.cap(1);
    QString message = regexp.cap(2);

    qDebug() << user + ": " + message;
}
