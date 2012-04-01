#ifndef __3DVisualSetEdgeColorExecutor_H__
#define __3DVisualSetEdgeColorExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class SetEdgeColorExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 17;
    };

}

#endif
