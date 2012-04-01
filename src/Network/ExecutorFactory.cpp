#include "Network/ExecutorFactory.h"

using namespace Network;

ExecutorFactory::ExecutorFactory() {

    executorList = QMap<quint8, AbstractExecutor*>();

    executorList.insert(UsersExecutor::INSTRUCTION_NUMBER, new UsersExecutor);
    executorList.insert(MoveNodeExecutor::INSTRUCTION_NUMBER, new MoveNodeExecutor);
    executorList.insert(GraphStartExecutor::INSTRUCTION_NUMBER, new GraphStartExecutor);
    executorList.insert(GraphEndExecutor::INSTRUCTION_NUMBER, new GraphEndExecutor);
    executorList.insert(NewNodeExecutor::INSTRUCTION_NUMBER, new NewNodeExecutor);
    executorList.insert(NewEdgeExecutor::INSTRUCTION_NUMBER, new NewEdgeExecutor);
    executorList.insert(LayoutExecutor::INSTRUCTION_NUMBER, new LayoutExecutor);
    executorList.insert(MoveAvatarExecutor::INSTRUCTION_NUMBER, new MoveAvatarExecutor);
    executorList.insert(IncommingUserExecutor::INSTRUCTION_NUMBER, new IncommingUserExecutor);
    executorList.insert(MessageExecutor::INSTRUCTION_NUMBER, new MessageExecutor);
    executorList.insert(ServerStopExecutor::INSTRUCTION_NUMBER, new ServerStopExecutor);
    executorList.insert(WelcomeExecutor::INSTRUCTION_NUMBER, new WelcomeExecutor);
    executorList.insert(SendGraphExecutor::INSTRUCTION_NUMBER, new SendGraphExecutor);
    executorList.insert(SpyUserExecutor::INSTRUCTION_NUMBER, new SpyUserExecutor);
    executorList.insert(UnspyUserExecutor::INSTRUCTION_NUMBER, new UnspyUserExecutor);
    executorList.insert(RemoveNodeExecutor::INSTRUCTION_NUMBER, new RemoveNodeExecutor);

}

AbstractExecutor* ExecutorFactory::getExecutor(QDataStream * stream) {

    quint8 instruction;
    *stream >> instruction;

    if (executorList.contains(instruction)) {

        AbstractExecutor * executor = executorList[instruction];
        executor->setDataStream(stream);
        return executor;

    } else {

        return NULL;

    }


}
