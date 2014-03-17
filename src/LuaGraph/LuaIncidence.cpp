#include "LuaGraph/LuaIncidence.h"

Lua::LuaIncidence::LuaIncidence()
{
}

qlonglong Lua::LuaIncidence::getId() const
{
    return id;
}

void Lua::LuaIncidence::setId(qlonglong value)
{
    id = value;
}

QString Lua::LuaIncidence::getLabel() const
{
    return label;
}

void Lua::LuaIncidence::setLabel(const QString &value)
{
    label = value;
}

void Lua::LuaIncidence::setEdgeNode(const qlonglong edgeId, const qlonglong nodeId)
{
    edgeNodePair.first = edgeId;
    edgeNodePair.second = nodeId;
}
QPair<qlonglong, qlonglong> Lua::LuaIncidence::getEdgeNodePair() const
{
    return edgeNodePair;
}

