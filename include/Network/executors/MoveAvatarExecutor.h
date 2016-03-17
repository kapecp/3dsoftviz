#ifndef __3DVisualMoveAvatarExecutor_H__
#define __3DVisualMoveAvatarExecutor_H__

#include <QByteArray>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

class MoveAvatarExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 10;
};

}

#endif
