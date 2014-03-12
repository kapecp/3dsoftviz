#include "LuaGraph/LuaIncidence.h"

Lua::LuaIncidence::LuaIncidence()
{
}

long Lua::LuaIncidence::getId() const
{
    return id;
}

void Lua::LuaIncidence::setId(long value)
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
