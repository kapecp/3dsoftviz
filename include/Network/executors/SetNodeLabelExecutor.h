#ifndef __3DVisualSetNodeLabelExecutor_H__
#define __3DVisualSetNodeLabelExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class SetNodeLabelExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 18;
    };

}

#endif
