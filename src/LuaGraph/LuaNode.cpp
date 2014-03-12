#include "LuaGraph/LuaNode.h"

Lua::LuaNode::LuaNode()
{
}

long Lua::LuaNode::getId() const
{
    return id;
}

void Lua::LuaNode::setId(long value)
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
