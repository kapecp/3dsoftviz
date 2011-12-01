#ifndef __3DVisualExecutorBuilder_H__
#define __3DVisualExecutorBuilder_H__

#include <QRegExp>
#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ExecutorFactory {

    public:
        ExecutorFactory();
        AbstractExecutor* getExecutor(QString line);

    private:
        QRegExp messageRegex;
        QRegExp usersRegex;
        QRegExp nodeRegexp;
        QRegExp layRegexp;
        QRegExp edgeRegexp;
        QRegExp moveNodeRegexp;
        QRegExp viewRegexp;
    };

}

#endif
