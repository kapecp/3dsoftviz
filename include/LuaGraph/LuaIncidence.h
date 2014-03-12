#ifndef LUAINCIDENCE_H
#define LUAINCIDENCE_H

#include "QPair"
#include "QString"

namespace Lua {

class LuaIncidence
{
public:
    LuaIncidence();
    long getId() const;
    void setId(long value);

    QString getLabel() const;
    void setLabel(const QString &value);

private:
    long id;
    QString label;
    QPair<long, long> edgeNodePair;
};

}

#endif // LUAINCIDENCE_H
