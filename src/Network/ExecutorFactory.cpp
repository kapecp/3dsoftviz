#include <typeinfo>
#include <QDebug>

#include "Network/ExecutorFactory.h"

using namespace Network;

ExecutorFactory::ExecutorFactory() {

    usersExecutor = new UsersExecutor;
    moveNodeExecutor = new MoveNodeExecutor;
    graphStartExecutor = new GraphStartExecutor;
    graphEndExecutor = new GraphEndExecutor;
    newNodeExecutor = new NewNodeExecutor;
    newEdgeExecutor = new NewEdgeExecutor;
    layoutExecutor = new LayoutExecutor;
    moveAvatarExecutor = new MoveAvatarExecutor;
    incommingUserExecutor = new IncommingUserExecutor;
    messageExecutor = new MessageExecutor;
    serverStopExecutor = new ServerStopExecutor;
    welcomeExecutor = new WelcomeExecutor;
    sendGraphExecutor = new SendGraphExecutor;
    spyUserExecutor = new SpyUserExecutor;
    unspyUserExecutor = new UnspyUserExecutor;
    removeNodeExecutor = new RemoveNodeExecutor;

}

AbstractExecutor* ExecutorFactory::getExecutor(QDataStream * stream) {

    quint8 instruction;
    *stream >> instruction;

    switch (instruction) {
    case IncommingUserExecutor::INSTRUCTION_NUMBER:
        incommingUserExecutor->setDataStream(stream);
        return incommingUserExecutor;
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
    case UnspyUserExecutor::INSTRUCTION_NUMBER :
        unspyUserExecutor->setDataStream(stream);
        return unspyUserExecutor;
        break;
    case SendGraphExecutor::INSTRUCTION_NUMBER :
        sendGraphExecutor->setDataStream(stream);
        return sendGraphExecutor;
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
