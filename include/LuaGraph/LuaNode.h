#ifndef LUANODE_H
#define LUANODE_H

#include "QList"
#include "QString"

namespace Lua {

class LuaNode
{
public:
    LuaNode();
    long getId() const;
    void setId(long value);

    QString getLabel() const;
    void setLabel(const QString &value);

private:
    long id;
    QString label;
    QList<long> incidences;
};

}

#endif // LUANODE_H
