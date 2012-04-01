#ifndef __3DVisualMessageExecutor_H__
#define __3DVisualMessageExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class MessageExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 13;
    };

}

#endif
