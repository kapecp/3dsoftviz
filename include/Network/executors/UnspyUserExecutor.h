#ifndef __3DVisualUnspyUserExecutor_H__
#define __3DVisualUnspyUserExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class UnspyUserExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        UnspyUserExecutor(QRegExp regex);
        void execute();
        void setVariables(QRegExp new_regexp) {regexp=new_regexp;}
    };

}

#endif
