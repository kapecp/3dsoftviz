#ifndef __3DVisualServerIncommingUserExecutor_H__
#define __3DVisualServerIncommingUserExecutor_H__

#include <QTcpSocket>
#include <QRegExp>

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerIncommingUserExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;
        QTcpSocket * senderClient;

    public:
        ServerIncommingUserExecutor(QRegExp regexp, QTcpSocket * senderClient);
        void execute();

    };

}

#endif
