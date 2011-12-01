#include "Network/ExecutorFactory.h"

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

using namespace Network;

ExecutorFactory::ExecutorFactory() {
    messageRegex = QRegExp("^([^:]+):(.*)$");

    usersRegex = QRegExp("^/clients:(.+)$");

    nodeRegexp = QRegExp("^/nodeData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    layRegexp = QRegExp("^/layData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    edgeRegexp = QRegExp("^/edgeData:id:([0-9]+);from:([0-9]+);to:([0-9]+);or:([01])$");

    moveNodeRegexp = QRegExp("^/moveNode:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    viewRegexp = QRegExp("^/view:center:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);rotation:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);id:([0-9]+)$");
}

AbstractExecutor* ExecutorFactory::getExecutor(QString line) {

    if (usersRegex.indexIn(line) != -1) {
        return new UsersExecutor(usersRegex);
    } else if (moveNodeRegexp.indexIn(line) != -1) {
        return new MoveNodeExecutor(moveNodeRegexp);
    } else if (line == "GRAPH_START") {
        return new GraphStartExecutor();
    } else if (line == "GRAPH_END") {
        return new GraphEndExecutor();
    } else if (nodeRegexp.indexIn(line) != -1) {
        return new NewNodeExecutor(nodeRegexp);
    } else if (edgeRegexp.indexIn(line) != -1) {
        return new NewEdgeExecutor(edgeRegexp);
    } else if (layRegexp.indexIn(line) != -1) {
        return new LayoutExecutor(layRegexp);
    } else if (viewRegexp.indexIn(line) != -1) {
        return new MoveAvatarExecutor(viewRegexp);
    } else if (messageRegex.indexIn(line) != -1 ) {
        return new MessageExecutor(messageRegex);
    } else if (line == "SERVER_STOP") {
        return new ServerStopExecutor();
    } else if (line == "WELCOME") {
        return new WelcomeExecutor();
    } else {
        return NULL;
    }




}
