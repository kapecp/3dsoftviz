#ifndef __3DVisualUnSetRestrictionExecutor_H__
#define __3DVisualUnSetRestrictionExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class UnSetRestrictionExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 26;
};

}

#endif
