#ifndef __3DVisualServerStopExecutor_H__
#define __3DVisualServerStopExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerStopExecutor : public AbstractExecutor {

    public:
        void execute();

    };

}

#endif
