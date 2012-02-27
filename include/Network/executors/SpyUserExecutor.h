#ifndef __3DVisualSpyUserExecutor_H__
#define __3DVisualSpyUserExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class SpyUserExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        SpyUserExecutor(QRegExp regex);
        void execute();
        void setVariables(QRegExp new_regexp) {regexp=new_regexp;}
    };

}

#endif
