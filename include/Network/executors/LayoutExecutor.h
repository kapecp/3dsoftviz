#ifndef __3DVisualLayoutExecutor_H__
#define __3DVisualLayoutExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class LayoutExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 10;
    };

}

#endif
