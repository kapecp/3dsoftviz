#ifndef __3DVisualMoveNodeExecutor_H__
#define __3DVisualMoveNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class MoveNodeExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 11;
    };

}

#endif
