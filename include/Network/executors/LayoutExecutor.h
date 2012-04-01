#ifndef __3DVisualLayoutExecutor_H__
#define __3DVisualLayoutExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class LayoutExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 10;
    };

}

#endif
