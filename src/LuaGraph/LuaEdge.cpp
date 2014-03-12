#include "LuaGraph/LuaEdge.h"

Lua::LuaEdge::LuaEdge()
{
}

long Lua::LuaEdge::getId() const
{
    return id;
}

void Lua::LuaEdge::setId(long value)
{
    id = value;
}

QString Lua::LuaEdge::getLabel() const
{
    return label;
}

void Lua::LuaEdge::setLabel(const QString &value)
{
    label = value;
}
