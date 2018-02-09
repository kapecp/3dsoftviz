#ifndef __3DVisualSendHandsExecutor_H__
#define __3DVisualSendHandsExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class SendHandsExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 28;

};

}

#endif
