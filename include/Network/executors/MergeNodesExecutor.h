#ifndef __3DVisualMergeNodesExecutor_H__
#define __3DVisualMergeNodesExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class MergeNodesExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 22;
};

}

#endif
