#ifndef __3DVisualServerRemoveNodeExecutor_H__
#define __3DVisualServerRemoveNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class ServerRemoveNodeExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 23;
    };

}

#endif
