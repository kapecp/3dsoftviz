#ifndef __3DVisualServerIncommingUserExecutor_H__
#define __3DVisualServerIncommingUserExecutor_H__

#include <QTcpSocket>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerIncommingUserExecutor : public AbstractExecutor {

    private:
        QDataStream * stream;
        QTcpSocket * out_socket;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        void setOutputSocket(QTcpSocket *output_socket) { out_socket = output_socket; }

        static const quint8 INSTRUCTION_NUMBER = 1;
    };

}

#endif
