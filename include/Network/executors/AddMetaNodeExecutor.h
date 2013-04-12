#ifndef __3DVisualAddMetaNodeExecutor_H__
#define __3DVisualAddMetaNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class AddMetaNodeExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 24;
    };

}

#endif
