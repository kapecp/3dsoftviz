#ifndef __3DVisualSeparateNodesExecutor_H__
#define __3DVisualSeparateNodesExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class SeparateNodesExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 23;
    };

}

#endif
