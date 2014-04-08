#include "LuaGraph/LuaIncidence.h"

void Lua::LuaIncidence::setEdgeNode(const qlonglong edgeId, const qlonglong nodeId)
{
    edgeNodePair.first = edgeId;
    edgeNodePair.second = nodeId;
}
QPair<qlonglong, qlonglong> Lua::LuaIncidence::getEdgeNodePair() const
{
    return edgeNodePair;
}

