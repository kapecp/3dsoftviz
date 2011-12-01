#include "Network/ExecutorFactory.h"

#include "Network/executors/AbstractExecutor.h"
#include "Network/executors/UsersExecutor.h"
#include "Network/executors/MoveNodeExecutor.h"
#include "Network/executors/GraphStartExecutor.h"
#include "Network/executors/GraphEndExecutor.h"
#include "Network/executors/NewNodeExecutor.h"
#include "Network/executors/NewEdgeExecutor.h"

using namespace Network;

AbstractExecutor* ExecutorFactory::getExecutor(QString line) {

    QRegExp messageRegex("^([^:]+):(.*)$");

    QRegExp usersRegex("^/clients:(.+)$");

    QRegExp nodeRegexp("^/nodeData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    QRegExp layRegexp("^/layData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    QRegExp edgeRegexp("^/edgeData:id:([0-9]+);from:([0-9]+);to:([0-9]+);or:([01])$");

    QRegExp moveNodeRegexp("^/moveNode:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    QRegExp viewRegexp("^/view:center:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);rotation:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);id:([0-9]+)$");

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
    } else {
        return NULL;
    }



}
