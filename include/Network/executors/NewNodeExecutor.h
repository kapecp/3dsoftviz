#ifndef __3DVisualNewNodeExecutor_H__
#define __3DVisualNewNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class NewNodeExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 5;
    };

}

#endif
