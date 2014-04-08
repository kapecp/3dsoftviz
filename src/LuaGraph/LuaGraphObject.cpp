#include "LuaGraph/LuaGraphObject.h"

qlonglong Lua::LuaGraphObject::getId() const
{
    return id;
}

void Lua::LuaGraphObject::setId(qlonglong value)
{
    id = value;
}

QString Lua::LuaGraphObject::getLabel() const
{
    return label;
}

void Lua::LuaGraphObject::setLabel(const QString &value)
{
    label = value;
}
Diluculum::LuaValue Lua::LuaGraphObject::getParams() const
{
    return params;
}

void Lua::LuaGraphObject::setParams(const Diluculum::LuaValue &value)
{
    params = value;
}


