#ifndef __3DVisualServerSendGraphExecutor_H__
#define __3DVisualServerSendGraphExecutor_H__

#include <QTcpSocket>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerSendGraphExecutor : public AbstractExecutor {

    private:
        QTcpSocket * senderClient;

    public:
        ServerSendGraphExecutor(QTcpSocket * senderClient);
        void execute();
        void setVariables(QTcpSocket * new_senderClient) {senderClient=new_senderClient;}
    };

}

#endif
