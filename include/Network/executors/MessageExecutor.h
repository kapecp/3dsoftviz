#ifndef __3DVisualMessageExecutor_H__
#define __3DVisualMessageExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class MessageExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 13;
    };

}

#endif
