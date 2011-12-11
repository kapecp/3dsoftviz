#ifndef __3DVisualExecutorBuilder_H__
#define __3DVisualExecutorBuilder_H__

#include <QObject>
#include <QRegExp>
#include <QTcpSocket>
#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ExecutorFactory {

    public:
        ExecutorFactory(QObject * client);
        AbstractExecutor* getExecutor(QString line);
        void setSenderClient(QTcpSocket *senderClient) {this->senderClient = senderClient;}

    private:
        QRegExp messageRegex;
        QRegExp usersRegex;
        QRegExp nodeRegexp;
        QRegExp layRegexp;
        QRegExp edgeRegexp;
        QRegExp moveNodeRegexp;
        QRegExp serverMoveNodeRegexp;
        QRegExp viewRegexp;
        QRegExp serverViewRegexp;
        QRegExp incommingUserRegex;

        QObject * client;

        QTcpSocket *senderClient;
    };

}

#endif
