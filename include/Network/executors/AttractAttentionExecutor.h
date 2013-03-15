#ifndef __3DVisualAttractAttentionExecutor_H__
#define __3DVisualAttractAttentionExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class AttractAttentionExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 27;

    };

}

#endif
