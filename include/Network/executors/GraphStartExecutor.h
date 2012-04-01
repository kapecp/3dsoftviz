#ifndef __3DVisualGraphStartExecutor_H__
#define __3DVisualGraphStartExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class GraphStartExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 4;

    };

}

#endif
