#ifndef LUAEDGE_H
#define LUAEDGE_H

#include "LuaGraphObject.h"

#include "QList"
#include "Q_LONG"

namespace Lua {

class LuaEdge : public LuaGraphObject
{
public:

    void addIncidence(qlonglong id);

    QList<qlonglong> getIncidences() const;

private:
    QList<qlonglong> incidences;
};

}

#endif // LUAEDGE_H
