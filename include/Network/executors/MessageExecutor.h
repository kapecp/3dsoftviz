#ifndef __3DVisualMessageExecutor_H__
#define __3DVisualMessageExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class MessageExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        MessageExecutor(QRegExp regex);
        void execute();
        void setVariables(QRegExp new_regexp) {regexp=new_regexp;}
    };

}

#endif
