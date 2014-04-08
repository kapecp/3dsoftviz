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
