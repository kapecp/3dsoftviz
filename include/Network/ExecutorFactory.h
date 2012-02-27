#ifndef __3DVisualExecutorBuilder_H__
#define __3DVisualExecutorBuilder_H__

#include <QObject>
#include <QRegExp>
#include <QTcpSocket>
#include "Network/executors/AbstractExecutor.h"

#include "Network/executors/AbstractExecutor.h"
#include "Network/executors/UsersExecutor.h"
#include "Network/executors/MoveNodeExecutor.h"
#include "Network/executors/GraphStartExecutor.h"
#include "Network/executors/GraphEndExecutor.h"
#include "Network/executors/NewNodeExecutor.h"
#include "Network/executors/NewEdgeExecutor.h"
#include "Network/executors/LayoutExecutor.h"
#include "Network/executors/MoveAvatarExecutor.h"
#include "Network/executors/MessageExecutor.h"
#include "Network/executors/ServerStopExecutor.h"
#include "Network/executors/WelcomeExecutor.h"
#include "Network/executors/ServerMoveNodeExecutor.h"
#include "Network/executors/ServerMoveAvatarExecutor.h"
#include "Network/executors/ServerIncommingUserExecutor.h"
#include "Network/executors/ServerSendGraphExecutor.h"
#include "Network/executors/ServerSendLayoutExecutor.h"
#include "Network/executors/ServerSpyUserExecutor.h"
#include "Network/executors/SpyUserExecutor.h"

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
        QRegExp welcomeRegex;
        QRegExp serverSpyRegex;
        QRegExp spyRegex;

        UsersExecutor *usersExecutor;
        MoveNodeExecutor *moveNodeExecutor;
        ServerMoveNodeExecutor *serverMoveNodeExecutor;
        GraphStartExecutor *graphStartExecutor;
        GraphEndExecutor *graphEndExecutor;
        NewNodeExecutor *newNodeExecutor;
        NewEdgeExecutor *newEdgeExecutor;
        LayoutExecutor *layoutExecutor;
        MoveAvatarExecutor *moveAvatarExecutor;
        ServerMoveAvatarExecutor *serverMoveAvatarExecutor;
        ServerIncommingUserExecutor *serverIncommingUserExecutor;
        MessageExecutor *messageExecutor;
        ServerStopExecutor *serverStopExecutor;
        WelcomeExecutor *welcomeExecutor;
        ServerSendGraphExecutor *serverSendGraphExecutor;
        ServerSendLayoutExecutor *serverSendLayoutExecutor;
        ServerSpyUserExecutor *serverSpUseryExecutor;
        SpyUserExecutor *spyUserExecutor;

        QObject * client;

        QTcpSocket *senderClient;
    };

}

#endif
