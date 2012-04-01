#ifndef __3DVisualRemoveEdgeExecutor_H__
#define __3DVisualRemoveEdgeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class RemoveEdgeExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 15;
    };

}

#endif
