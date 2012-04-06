#ifndef __3DVisualMoveNodeExecutor_H__
#define __3DVisualMoveNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"
#include "Data/Node.h"

namespace Network {

    class MoveNodeExecutor : public AbstractExecutor {
    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 11;

    private:
        void moveNode(Data::Node *node, osg::Vec3 position);
    };

}

#endif
