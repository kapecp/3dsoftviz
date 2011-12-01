#ifndef __3DVisualNewNodeExecutor_H__
#define __3DVisualNewNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class NewNodeExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        NewNodeExecutor(QRegExp regex);
        void execute();

    };

}

#endif
