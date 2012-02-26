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
        void setVariables(QRegExp new_regexp) {regexp=new_regexp;}
    };

}

#endif
