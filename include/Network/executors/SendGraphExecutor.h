#ifndef __3DVisualSendGraphExecutor_H__
#define __3DVisualSendGraphExecutor_H__

#include <QTcpSocket>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class SendGraphExecutor : public AbstractExecutor {

    public:
        void execute_server();
        void execute_client();
        static const quint8 INSTRUCTION_NUMBER = 4;
    };

}

#endif
