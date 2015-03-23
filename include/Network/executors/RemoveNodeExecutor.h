#ifndef __3DVisualRemoveNodeExecutor_H__
#define __3DVisualRemoveNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class RemoveNodeExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 14;
};

}

#endif
