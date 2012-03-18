#ifndef __3DVisualSpyUserExecutor_H__
#define __3DVisualSpyUserExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class SpyUserExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 15;
    };

}

#endif
