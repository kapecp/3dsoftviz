#include <typeinfo>
#include <QDebug>

#include "Network/ExecutorFactory.h"

using namespace Network;

ExecutorFactory::ExecutorFactory(QObject * client) {
    messageRegex = QRegExp("^([^:]+):(.*)$");

    usersRegex = QRegExp("^/clients:(.+)$");

    nodeRegexp = QRegExp("^/nodeData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    layRegexp = QRegExp("^/layData:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    edgeRegexp = QRegExp("^/edgeData:id:([0-9]+);from:([0-9]+);to:([0-9]+);or:([01])$");

    moveNodeRegexp = QRegExp("^/moveNode:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    serverMoveNodeRegexp = QRegExp("^/SMoveNode:id:([0-9]+);x:([0-9-\\.e]+);y:([0-9-\\.e]+);z:([0-9-\\.e]+)$");

    viewRegexp = QRegExp("^/view:center:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);rotation:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);id:([0-9]+)$");

    serverViewRegexp = QRegExp("^/view:center:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+);rotation:([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+),([0-9-\\.e]+)$");

    incommingUserRegex = QRegExp("^/me:(.*)$");

    this->client = client;
    this->senderClient = NULL;

    usersExecutor = NULL;
    moveNodeExecutor = NULL;
    serverMoveNodeExecutor = NULL;
    graphStartExecutor = NULL;
    graphEndExecutor = NULL;
    newNodeExecutor = NULL;
    newEdgeExecutor = NULL;
    layoutExecutor = NULL;
    moveAvatarExecutor = NULL;
    serverMoveAvatarExecutor = NULL;
    serverIncommingUserExecutor = NULL;
    messageExecutor = NULL;
    serverStopExecutor = NULL;
    welcomeExecutor = NULL;
    serverSendGraphExecutor = NULL;
    serverSendLayoutExecutor = NULL;
}

AbstractExecutor* ExecutorFactory::getExecutor(QString line) {

    if (usersRegex.indexIn(line) != -1) {
        if (usersExecutor == NULL) {
            usersExecutor = new UsersExecutor(usersRegex);
        } else {
            usersExecutor->setVariables(usersRegex);
        }
        return usersExecutor;
    } else if (moveNodeRegexp.indexIn(line) != -1) {
        if (moveNodeExecutor == NULL) {
            moveNodeExecutor = new MoveNodeExecutor(moveNodeRegexp);
        } else {
            qDebug()<<"MoveNode exists";
            moveNodeExecutor->setVariables(moveNodeRegexp);
        }
        return moveNodeExecutor;
    } else if (serverMoveNodeRegexp.indexIn(line) != -1){
        if (serverMoveNodeExecutor == NULL) {
            serverMoveNodeExecutor = new ServerMoveNodeExecutor(serverMoveNodeRegexp);
        } else {
            serverMoveNodeExecutor->setVariables(serverMoveNodeRegexp);
        }
        return serverMoveNodeExecutor;
    }else if (line == "GRAPH_START") {
        if (graphStartExecutor == NULL) {
            graphStartExecutor = new GraphStartExecutor();
        }
        return graphStartExecutor;
    } else if (line == "GRAPH_END") {
        if (graphEndExecutor == NULL) {
            graphEndExecutor = new GraphEndExecutor();
        }
        return graphEndExecutor;
    } else if (nodeRegexp.indexIn(line) != -1) {
        if (newNodeExecutor == NULL) {
            newNodeExecutor = new NewNodeExecutor(nodeRegexp);
        } else {
            newNodeExecutor->setVariables(nodeRegexp);
        }
        return newNodeExecutor;
    } else if (edgeRegexp.indexIn(line) != -1) {
        if (newEdgeExecutor == NULL) {
            newEdgeExecutor = new NewEdgeExecutor(edgeRegexp);
        } else {
            newEdgeExecutor->setVariables(edgeRegexp);
        }
        return newEdgeExecutor;
    } else if (layRegexp.indexIn(line) != -1) {
        if (layoutExecutor == NULL) {
            layoutExecutor = new LayoutExecutor(layRegexp);
        } else {
            layoutExecutor->setVariables(layRegexp);
        }
        return layoutExecutor;
    } else if (viewRegexp.indexIn(line) != -1) {
        if (moveAvatarExecutor == NULL) {
            moveAvatarExecutor = new MoveAvatarExecutor(viewRegexp);
        } else {
            moveAvatarExecutor->setVariables(viewRegexp);
        }
        return moveAvatarExecutor;
    } else if (serverViewRegexp.indexIn(line) != -1 && senderClient != NULL) {
        if (serverMoveAvatarExecutor == NULL) {
            serverMoveAvatarExecutor = new ServerMoveAvatarExecutor(serverViewRegexp, senderClient, line);
        } else {
            serverMoveAvatarExecutor->setVariables(serverViewRegexp, senderClient, line);
        }
        return serverMoveAvatarExecutor;
    } else if (incommingUserRegex.indexIn(line) != -1 && senderClient != NULL) {
        if (serverIncommingUserExecutor == NULL) {
            serverIncommingUserExecutor = new ServerIncommingUserExecutor(incommingUserRegex, senderClient);
        } else {
            serverIncommingUserExecutor->setVariables(incommingUserRegex, senderClient);
        }
        return serverIncommingUserExecutor;
    } else if (messageRegex.indexIn(line) != -1 ) {
        if (messageExecutor == NULL) {
            messageExecutor = new MessageExecutor(messageRegex);
        } else {
            messageExecutor->setVariables(messageRegex);
        }
        return messageExecutor;
    } else if (line == "SERVER_STOP") {
        if (serverStopExecutor == NULL) {
            serverStopExecutor = new ServerStopExecutor();
        }
        return serverStopExecutor;
    } else if (line == "WELCOME") {
        if (welcomeExecutor == NULL) {
            welcomeExecutor = new WelcomeExecutor();
        }
        return welcomeExecutor;
    } else if (line == "GET_GRAPH" && senderClient != NULL){
        if (serverSendGraphExecutor == NULL) {
            serverSendGraphExecutor = new ServerSendGraphExecutor(senderClient);
        } else {
            serverSendGraphExecutor->setVariables(senderClient);
        }
        return serverSendGraphExecutor;
    } else if (line == "GET_LAYOUT" && senderClient != NULL){
        if (serverSendLayoutExecutor == NULL) {
            serverSendLayoutExecutor = new ServerSendLayoutExecutor(senderClient);
        } else {
            serverSendLayoutExecutor->setVariables(senderClient);
        }
        return serverSendLayoutExecutor;
    } else {
        return NULL;
    }




}
