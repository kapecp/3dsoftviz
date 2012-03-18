#ifndef __3DVisualUnspyUserExecutor_H__
#define __3DVisualUnspyUserExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class UnspyUserExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 17;
    };

}

#endif
