#ifndef __3DVisualIncommingUserExecutor_H__
#define __3DVisualIncommingUserExecutor_H__

#include <QByteArray>
#include <QTcpSocket>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class IncommingUserExecutor : public AbstractExecutor
{

public:
	void execute_server();
	void execute_client();
	static const quint8 INSTRUCTION_NUMBER = 1;
};

}

#endif
