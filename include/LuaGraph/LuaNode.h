#ifndef LUANODE_H
#define LUANODE_H

#include "QList"
#include "Q_LONG"
#include "QString"
#include "LuaGraphObject.h"

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
