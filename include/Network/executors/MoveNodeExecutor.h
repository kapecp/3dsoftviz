#ifndef __3DVisualMoveNodeExecutor_H__
#define __3DVisualMoveNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class MoveNodeExecutor : public AbstractExecutor {
    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 11;
    };

}

#endif
