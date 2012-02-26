#ifndef __3DVisualServerSendLayoutExecutor_H__
#define __3DVisualServerSendLayoutExecutor_H__

#include <QTcpSocket>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerSendLayoutExecutor : public AbstractExecutor {

    private:
        QTcpSocket * senderClient;

    public:
        ServerSendLayoutExecutor(QTcpSocket * senderClient);
        void execute();
        void setVariables(QTcpSocket * new_senderClient) {senderClient=new_senderClient;}
    };

}

#endif
