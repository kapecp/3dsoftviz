#ifndef LUAINCIDENCE_H
#define LUAINCIDENCE_H

#include "QPair"
#include "Q_LONG"
#include "QString"
#include "LuaGraphObject.h"

namespace Lua {

class LuaIncidence : public LuaGraphObject
{
public:
    void setEdgeNode(const qlonglong edgeId, const qlonglong nodeId);

    QPair<qlonglong, qlonglong> getEdgeNodePair() const;

private:
    QPair<qlonglong, qlonglong> edgeNodePair;
};

}

#endif // LUAINCIDENCE_H
