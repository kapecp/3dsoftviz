#ifndef __3DVisualMoveAvatarExecutor_H__
#define __3DVisualMoveAvatarExecutor_H__

#include "Network/executors/AbstractExecutor.h"

#include <QRegExp>

namespace Network {

    class MoveAvatarExecutor : public AbstractExecutor {

    private:
        QRegExp regexp;

    public:
        MoveAvatarExecutor(QRegExp regex);
        void execute();

    };

}

#endif
