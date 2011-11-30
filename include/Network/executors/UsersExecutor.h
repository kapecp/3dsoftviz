#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class UsersExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        UsersExecutor(QRegExp regex);
        void execute();

    };

}
