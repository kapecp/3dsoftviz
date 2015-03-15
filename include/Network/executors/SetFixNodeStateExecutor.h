#ifndef __3DVisualSetFixNodeStateExecutor_H__
#define __3DVisualSetFixNodeStateExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Data {
class Node;
}

namespace Network {

class SetFixNodeStateExecutor : public AbstractExecutor
{

public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 21;

private:
	void SetFixNodeState( Data::Node* node, bool state );
};

}

#endif
