#ifndef __3DVisualServerStopExecutor_H__
#define __3DVisualServerStopExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerStopExecutor : public AbstractExecutor {

    public:
        void execute();
        static const quint8 INSTRUCTION_NUMBER = 14;

    };

}

#endif
