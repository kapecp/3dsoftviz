#ifndef __3DVisualServerIncommingUserExecutor_H__
#define __3DVisualServerIncommingUserExecutor_H__

#include <QTcpSocket>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerIncommingUserExecutor : public AbstractExecutor {

    private:
        QDataStream * stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}

        static const quint8 INSTRUCTION_NUMBER = 1;
    };

}

#endif
