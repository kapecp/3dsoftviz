#ifndef __3DVisualMoveAvatarExecutor_H__
#define __3DVisualMoveAvatarExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class MoveAvatarExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 9;
    };

}

#endif
