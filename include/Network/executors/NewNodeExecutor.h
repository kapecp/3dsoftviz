#ifndef __3DVisualNewNodeExecutor_H__
#define __3DVisualNewNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class NewNodeExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 5;
    };

}

#endif
