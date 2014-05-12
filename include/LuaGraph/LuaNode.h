#ifndef LUANODE_H
#define LUANODE_H

#include "LuaGraphObject.h"

#include "QList"
#include "Q_LONG"

namespace Lua {

class LuaNode : public LuaGraphObject
{
public:
    void addIncidence(const qlonglong id);

    QList<qlonglong> getIncidences() const;

private:
    QList<qlonglong> incidences;

};

}

#endif // LUANODE_H
