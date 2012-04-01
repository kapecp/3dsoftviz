#ifndef __3DVisualRemoveNodeExecutor_H__
#define __3DVisualRemoveNodeExecutor_H__

#include "Network/executors/AbstractExecutor.h"

namespace Network {

    class RemoveNodeExecutor : public AbstractExecutor {

    private:
        QDataStream *stream;

    public:
        void execute();
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER = 22;
    };

}

#endif
