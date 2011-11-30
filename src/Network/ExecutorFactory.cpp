#include "Network/ExecutorFactory.h"

#include "Network/executors/AbstractExecutor.h"
#include "Network/executors/UsersExecutor.h"

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
    } else {
        return NULL;
    }



}
