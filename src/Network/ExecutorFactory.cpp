#include <typeinfo>
#include <QDebug>

#include "Network/ExecutorFactory.h"

using namespace Network;

ExecutorFactory::ExecutorFactory() {

    usersExecutor = new UsersExecutor;
    moveNodeExecutor = new MoveNodeExecutor;
    serverMoveNodeExecutor = new ServerMoveNodeExecutor;
    graphStartExecutor = new GraphStartExecutor;
    graphEndExecutor = new GraphEndExecutor;
    newNodeExecutor = new NewNodeExecutor;
    newEdgeExecutor = new NewEdgeExecutor;
    layoutExecutor = new LayoutExecutor;
    moveAvatarExecutor = new MoveAvatarExecutor;
    serverMoveAvatarExecutor = new ServerMoveAvatarExecutor;
    serverIncommingUserExecutor = new ServerIncommingUserExecutor;
    messageExecutor = new MessageExecutor;
    serverStopExecutor = new ServerStopExecutor;
    welcomeExecutor = new WelcomeExecutor;
    serverSendGraphExecutor = new ServerSendGraphExecutor;
    serverSendLayoutExecutor = NULL;
    serverSpyUserExecutor = new ServerSpyUserExecutor;
    spyUserExecutor = new SpyUserExecutor;
    serverUnspyUserExecutor = new ServerUnspyUserExecutor;
    unspyUserExecutor = new UnspyUserExecutor;
    serverNewNodeExecutor = new ServerNewNodeExecutor;
    serverNewEdgeExecutor = new ServerNewEdgeExecutor;
    removeNodeExecutor = new RemoveNodeExecutor;

}

AbstractExecutor* ExecutorFactory::getExecutor(QDataStream * stream) {

    quint8 instruction;
    *stream >> instruction;

    switch (instruction) {
    case ServerIncommingUserExecutor::INSTRUCTION_NUMBER:
        serverIncommingUserExecutor->setDataStream(stream);
        return serverIncommingUserExecutor;
        break;
    case WelcomeExecutor::INSTRUCTION_NUMBER :
        welcomeExecutor->setDataStream(stream);
        return welcomeExecutor;
        break;
    case UsersExecutor::INSTRUCTION_NUMBER :
        usersExecutor->setDataStream(stream);
        return usersExecutor;
        break;
    case GraphStartExecutor::INSTRUCTION_NUMBER :
        return graphStartExecutor;
        break;
    case NewNodeExecutor::INSTRUCTION_NUMBER :
        newNodeExecutor->setDataStream(stream);
        return newNodeExecutor;
        break;
    case NewEdgeExecutor::INSTRUCTION_NUMBER :
        newEdgeExecutor->setDataStream(stream);
        return newEdgeExecutor;
        break;
    case GraphEndExecutor::INSTRUCTION_NUMBER :
        return graphEndExecutor;
        break;
    case ServerMoveAvatarExecutor::INSTRUCTION_NUMBER :
        serverMoveAvatarExecutor->setDataStream(stream);
        return serverMoveAvatarExecutor;
        break;
    case MoveAvatarExecutor::INSTRUCTION_NUMBER :
        moveAvatarExecutor->setDataStream(stream);
        return moveAvatarExecutor;
        break;
    case LayoutExecutor::INSTRUCTION_NUMBER :
        layoutExecutor->setDataStream(stream);
        return layoutExecutor;
        break;
    case MoveNodeExecutor::INSTRUCTION_NUMBER :
        moveNodeExecutor->setDataStream(stream);
        return moveNodeExecutor;
        break;
    case ServerMoveNodeExecutor::INSTRUCTION_NUMBER :
        serverMoveNodeExecutor->setDataStream(stream);
        return serverMoveNodeExecutor;
        break;
    case MessageExecutor::INSTRUCTION_NUMBER :
        messageExecutor->setDataStream(stream);
        return messageExecutor;
        break;
    case ServerStopExecutor::INSTRUCTION_NUMBER :
        return serverStopExecutor;
        break;
    case SpyUserExecutor::INSTRUCTION_NUMBER :
        spyUserExecutor->setDataStream(stream);
        return spyUserExecutor;
        break;
    case ServerSpyUserExecutor::INSTRUCTION_NUMBER :
        serverSpyUserExecutor->setDataStream(stream);
        return serverSpyUserExecutor;
        break;
    case UnspyUserExecutor::INSTRUCTION_NUMBER :
        unspyUserExecutor->setDataStream(stream);
        return unspyUserExecutor;
        break;
    case ServerUnspyUserExecutor::INSTRUCTION_NUMBER :
        serverUnspyUserExecutor->setDataStream(stream);
        return serverUnspyUserExecutor;
        break;
    case ServerSendGraphExecutor::INSTRUCTION_NUMBER :
        serverSendGraphExecutor->setDataStream(stream);
        return serverSendGraphExecutor;
        break;
    case ServerNewNodeExecutor::INSTRUCTION_NUMBER :
        serverNewNodeExecutor->setDataStream(stream);
        return serverNewNodeExecutor;
        break;
    case ServerNewEdgeExecutor::INSTRUCTION_NUMBER :
        serverNewEdgeExecutor->setDataStream(stream);
        return serverNewEdgeExecutor;
        break;
    case RemoveNodeExecutor::INSTRUCTION_NUMBER :
        removeNodeExecutor->setDataStream(stream);
        return removeNodeExecutor;
        break;
    default:
        return NULL;
        break;
    }

}
