#ifndef LUAGRAPHOBJECT_H
#define LUAGRAPHOBJECT_H

#include "Diluculum/LuaValue.hpp"

namespace Lua {

class LuaGraphObject
{
public:
    qlonglong getId() const;
    void setId(qlonglong value);

    QString getLabel() const;
    void setLabel(const QString &value);

    Diluculum::LuaValue getParams() const;
    void setParams(const Diluculum::LuaValue &value);

private:
    qlonglong id;
    QString label;
    Diluculum::LuaValue params;
};

}

#endif // LUAGRAPHOBJECT_H
