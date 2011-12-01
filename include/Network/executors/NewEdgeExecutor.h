#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class NewEdgeExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        NewEdgeExecutor(QRegExp regex);
        void execute();

    };

}
