#ifndef __3DVisualWelcomeExecutor_H__
#define __3DVisualWelcomeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class WelcomeExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 2;

    };

}

#endif
