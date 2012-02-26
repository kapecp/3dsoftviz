#ifndef __3DVisualNewEdgeExecutor_H__
#define __3DVisualNewEdgeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class NewEdgeExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        NewEdgeExecutor(QRegExp regex);
        void execute();
        void setVariables(QRegExp new_regexp) {regexp=new_regexp;}
    };

}

#endif
