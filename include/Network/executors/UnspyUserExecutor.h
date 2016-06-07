#ifndef __3DVisualUnspyUserExecutor_H__
#define __3DVisualUnspyUserExecutor_H__

#include <QByteArray>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class UnspyUserExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 13;
};

}

#endif
