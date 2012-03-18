#ifndef __3DVisualServerMoveAvatarExecutor_H__
#define __3DVisualServerMoveAvatarExecutor_H__

#include <QTcpSocket>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerMoveAvatarExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}

        static const quint8 INSTRUCTION_NUMBER = 8;
    };

}

#endif
