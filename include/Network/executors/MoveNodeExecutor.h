#ifndef __3DVisualMoveNodeExecutor_H__
#define __3DVisualMoveNodeExecutor_H__

#include <QByteArray>

#include "Network/executors/AbstractExecutor.h"

namespace Data {
class Node;
}

namespace Network {

class MoveNodeExecutor : public AbstractExecutor
{
public:
	void execute_client();
	void execute_server();
	static const quint8 INSTRUCTION_NUMBER = 11;

private:
	void moveNode( Data::Node* node, osg::Vec3 position );
};

}

#endif
