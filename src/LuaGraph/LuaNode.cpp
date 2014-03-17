#include "LuaGraph/LuaNode.h"

Lua::LuaNode::LuaNode()
{
}

qlonglong Lua::LuaNode::getId() const
{
    return id;
}

void Lua::LuaNode::setId(qlonglong value)
{
    id = value;
}

QString Lua::LuaNode::getLabel() const
{
    return label;
}

void Lua::LuaNode::setLabel(const QString &value)
{
    label = value;
}

void Lua::LuaNode::addIncidence(const qlonglong id)
{
    incidences.append(id);
}

QList<qlonglong> Lua::LuaNode::getIncidences() const
{
    return incidences;
}
