#include "Network/ExecutorFactory.h"

namespace Network {

ExecutorFactory::ExecutorFactory()
{

	executorList = QMap<quint8, AbstractExecutor*>();
	executorList.insert( quint8( IncommingUserExecutor::INSTRUCTION_NUMBER ), new IncommingUserExecutor );
	executorList.insert( quint8( WelcomeExecutor::INSTRUCTION_NUMBER ), new WelcomeExecutor );
	executorList.insert( quint8( UsersExecutor::INSTRUCTION_NUMBER ), new UsersExecutor );
	executorList.insert( quint8( SendGraphExecutor::INSTRUCTION_NUMBER ), new SendGraphExecutor );
	executorList.insert( quint8( GraphStartExecutor::INSTRUCTION_NUMBER ), new GraphStartExecutor );
	executorList.insert( quint8( NewNodeExecutor::INSTRUCTION_NUMBER ), new NewNodeExecutor );
	executorList.insert( quint8( NewEdgeExecutor::INSTRUCTION_NUMBER ), new NewEdgeExecutor );
	executorList.insert( quint8( GraphEndExecutor::INSTRUCTION_NUMBER ), new GraphEndExecutor );
	executorList.insert( quint8( LayoutExecutor::INSTRUCTION_NUMBER ), new LayoutExecutor );
	executorList.insert( quint8( MoveAvatarExecutor::INSTRUCTION_NUMBER ), new MoveAvatarExecutor );
	executorList.insert( quint8( MoveNodeExecutor::INSTRUCTION_NUMBER ), new MoveNodeExecutor );
	executorList.insert( quint8( SpyUserExecutor::INSTRUCTION_NUMBER ), new SpyUserExecutor );
	executorList.insert( quint8( UnspyUserExecutor::INSTRUCTION_NUMBER ), new UnspyUserExecutor );
	executorList.insert( quint8( RemoveNodeExecutor::INSTRUCTION_NUMBER ), new RemoveNodeExecutor );
	executorList.insert( quint8( RemoveEdgeExecutor::INSTRUCTION_NUMBER ), new RemoveEdgeExecutor );
	executorList.insert( quint8( SetNodeColorExecutor::INSTRUCTION_NUMBER ), new SetNodeColorExecutor );
	executorList.insert( quint8( SetEdgeColorExecutor::INSTRUCTION_NUMBER ), new SetEdgeColorExecutor );
	executorList.insert( quint8( SetNodeLabelExecutor::INSTRUCTION_NUMBER ), new SetNodeLabelExecutor );
	executorList.insert( quint8( MessageExecutor::INSTRUCTION_NUMBER ), new MessageExecutor );
	executorList.insert( quint8( ServerStopExecutor::INSTRUCTION_NUMBER ), new ServerStopExecutor );
	executorList.insert( quint8( SetFixNodeStateExecutor::INSTRUCTION_NUMBER ), new SetFixNodeStateExecutor );
	executorList.insert( quint8( MergeNodesExecutor::INSTRUCTION_NUMBER ), new MergeNodesExecutor );
	executorList.insert( quint8( SeparateNodesExecutor::INSTRUCTION_NUMBER ), new SeparateNodesExecutor );
	executorList.insert( quint8( AddMetaNodeExecutor::INSTRUCTION_NUMBER ), new AddMetaNodeExecutor );
	executorList.insert( quint8( SetRestrictionExecutor::INSTRUCTION_NUMBER ), new SetRestrictionExecutor );
	executorList.insert( quint8( UnSetRestrictionExecutor::INSTRUCTION_NUMBER ), new UnSetRestrictionExecutor );
	executorList.insert( quint8( AttractAttentionExecutor::INSTRUCTION_NUMBER ), new AttractAttentionExecutor );
	executorList.insert( quint8( SendHandsExecutor::INSTRUCTION_NUMBER ), new SendHandsExecutor );

}

AbstractExecutor* ExecutorFactory::getExecutor( QDataStream* stream )
{

	quint8 instruction;
	*stream >> instruction;

	if ( executorList.contains( instruction ) ) {

		AbstractExecutor* executor = executorList[instruction];
		executor->setDataStream( stream );
		return executor;

	}
	else {

		return NULL;

	}


}

} // namespace Network
