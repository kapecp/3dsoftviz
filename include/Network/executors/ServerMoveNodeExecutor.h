#ifndef __3DVisualServerMoveNodeExecutor_H__
#define __3DVisualServerMoveNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerMoveNodeExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 12;
    };

}

#endif
