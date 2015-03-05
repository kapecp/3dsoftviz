#ifndef __3DVisualSetNodeColorExecutor_H__
#define __3DVisualSetNodeColorExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class SetNodeColorExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 16;
};

}

#endif
