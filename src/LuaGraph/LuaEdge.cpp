#include "LuaGraph/LuaEdge.h"

Lua::LuaEdge::LuaEdge()
{
}

qlonglong Lua::LuaEdge::getId() const
{
    return id;
}

void Lua::LuaEdge::setId(qlonglong value)
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

void Lua::LuaEdge::addIncidence(qlonglong id)
{
    incidences.append(id);
}

QList<qlonglong> Lua::LuaEdge::getIncidences() const
{
    return incidences;
}
