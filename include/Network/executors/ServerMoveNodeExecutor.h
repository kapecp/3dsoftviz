#ifndef __3DVisualServerMoveNodeExecutor_H__
#define __3DVisualServerMoveNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class ServerMoveNodeExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        ServerMoveNodeExecutor(QRegExp regex);
        void execute();

    };

}

#endif
