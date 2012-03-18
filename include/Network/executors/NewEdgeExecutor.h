#ifndef __3DVisualNewEdgeExecutor_H__
#define __3DVisualNewEdgeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class NewEdgeExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 6;
    };

}

#endif
