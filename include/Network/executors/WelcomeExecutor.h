#ifndef __3DVisualWelcomeExecutor_H__
#define __3DVisualWelcomeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class WelcomeExecutor : public AbstractExecutor {

    public:
        void execute_client();
        void execute_server();
        static const quint8 INSTRUCTION_NUMBER = 2;

    };

}

#endif
