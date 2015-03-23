#ifndef __3DVisualSetRestrictionExecutor_H__
#define __3DVisualSetRestrictionExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class SetRestrictionExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 25;
};

}

#endif
