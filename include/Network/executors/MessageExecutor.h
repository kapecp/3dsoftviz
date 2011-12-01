#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class MessageExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        MessageExecutor(QRegExp regex);
        void execute();

    };

}
