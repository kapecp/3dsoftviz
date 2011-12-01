#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class MoveNodeExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        MoveNodeExecutor(QRegExp regex);
        void execute();

    };

}
