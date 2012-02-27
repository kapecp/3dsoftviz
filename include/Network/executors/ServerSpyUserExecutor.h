#ifndef __3DVisualServerSpyUserExecutor_H__
#define __3DVisualServerSpyUserExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>
#include <QTcpSocket>

namespace Network {

    class ServerSpyUserExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;
        QTcpSocket *senderClient;
        QString line;

    public:
        ServerSpyUserExecutor(QRegExp regex, QTcpSocket *senderClient, QString line);
        void execute();
        void setVariables(QRegExp new_regexp, QTcpSocket *new_senderClient, QString new_line)
                            {regexp=new_regexp; senderClient=new_senderClient;line=new_line;}
    };

}

#endif
