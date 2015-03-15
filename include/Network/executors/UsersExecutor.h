#ifndef __3DVisualUsersExecutor_H__
#define __3DVisualUsersExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class UsersExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 3;
};

}

#endif
