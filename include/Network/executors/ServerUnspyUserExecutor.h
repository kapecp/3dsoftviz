#ifndef __3DVisualServerUnspyUserExecutor_H__
#define __3DVisualServerUnspyUserExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerUnspyUserExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 18;
    };
}

#endif
