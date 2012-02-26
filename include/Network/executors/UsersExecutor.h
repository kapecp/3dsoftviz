#ifndef __3DVisualUsersExecutor_H__
#define __3DVisualUsersExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class UsersExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        UsersExecutor(QRegExp regex);
        void execute();
        void setVariables(QRegExp new_regexp) {regexp=new_regexp;}
    };

}

#endif
