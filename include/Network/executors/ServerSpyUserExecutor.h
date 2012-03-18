#ifndef __3DVisualServerSpyUserExecutor_H__
#define __3DVisualServerSpyUserExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerSpyUserExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 16;
    };

}

#endif
