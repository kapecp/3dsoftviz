#ifndef __3DVisualExecutorBuilder_H__
#define __3DVisualExecutorBuilder_H__

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
#include "Network/executors/SendHandsExecutor.h"
#include "Network/executors/SpyUserExecutor.h"
#include "Network/executors/UnspyUserExecutor.h"
#include "Network/executors/RemoveNodeExecutor.h"
#include "Network/executors/RemoveEdgeExecutor.h"
#include "Network/executors/SetNodeColorExecutor.h"
#include "Network/executors/SetEdgeColorExecutor.h"
#include "Network/executors/SetNodeLabelExecutor.h"
#include "Network/executors/SetFixNodeStateExecutor.h"
#include "Network/executors/MergeNodesExecutor.h"
#include "Network/executors/SeparateNodesExecutor.h"
#include "Network/executors/AddMetaNodeExecutor.h"
#include "Network/executors/SetRestrictionExecutor.h"
#include "Network/executors/UnSetRestrictionExecutor.h"
#include "Network/executors/AttractAttentionExecutor.h"

namespace Network {

class ExecutorFactory
{

public:
	ExecutorFactory();
	AbstractExecutor* getExecutor( QDataStream* stream );

private:

	QMap<quint8, AbstractExecutor*> executorList;
};

}

#endif
