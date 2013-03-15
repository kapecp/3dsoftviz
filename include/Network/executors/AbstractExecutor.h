#ifndef __3DVisualExecutor_H__
#define __3DVisualExecutor_H__

#include <QObject>

namespace Network {

    class AbstractExecutor {

    protected:
        QDataStream *stream;

    public:

        virtual void execute_client() = 0;
        virtual void execute_server() = 0;
        void setDataStream(QDataStream *ds) {stream=ds;}
        static const quint8 INSTRUCTION_NUMBER;

    };

}

#endif
