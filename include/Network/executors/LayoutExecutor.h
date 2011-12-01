#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class LayoutExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        LayoutExecutor(QRegExp regex);
        void execute();

    };

}
