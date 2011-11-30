#ifndef __3DVisualExecutorBuilder_H__
#define __3DVisualExecutorBuilder_H__

#include <QString>
#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ExecutorFactory {

    public:
        AbstractExecutor* getExecutor(QString line);

    };

}

#endif
