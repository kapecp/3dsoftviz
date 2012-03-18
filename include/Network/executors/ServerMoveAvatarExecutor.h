#ifndef __3DVisualServerMoveAvatarExecutor_H__
#define __3DVisualServerMoveAvatarExecutor_H__

#include <QTcpSocket>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerMoveAvatarExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;
        QTcpSocket * out_socket;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        void setOutputSocket(QTcpSocket *output_socket) { out_socket = output_socket; }

        static const quint8 INSTRUCTION_NUMBER = 8;
    };

}

#endif
