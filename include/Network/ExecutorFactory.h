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
#include "Network/executors/IncommingUserExecutor.h"
#include "Network/executors/SendGraphExecutor.h"
#include "Network/executors/SpyUserExecutor.h"
#include "Network/executors/UnspyUserExecutor.h"
#include "Network/executors/RemoveNodeExecutor.h"

namespace Network {

    class ExecutorFactory {

    public:
        ExecutorFactory();
        AbstractExecutor* getExecutor(QDataStream * stream);

    private:
        UsersExecutor *usersExecutor;
        MoveNodeExecutor *moveNodeExecutor;
        GraphStartExecutor *graphStartExecutor;
        GraphEndExecutor *graphEndExecutor;
        NewNodeExecutor *newNodeExecutor;
        NewEdgeExecutor *newEdgeExecutor;
        LayoutExecutor *layoutExecutor;
        MoveAvatarExecutor *moveAvatarExecutor;
        IncommingUserExecutor *incommingUserExecutor;
        MessageExecutor *messageExecutor;
        ServerStopExecutor *serverStopExecutor;
        WelcomeExecutor *welcomeExecutor;
        SendGraphExecutor *sendGraphExecutor;
        SpyUserExecutor *spyUserExecutor;
        UnspyUserExecutor * unspyUserExecutor;
        RemoveNodeExecutor *removeNodeExecutor;
    };

}

#endif
