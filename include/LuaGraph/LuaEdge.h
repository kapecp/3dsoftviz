#ifndef LUAEDGE_H
#define LUAEDGE_H

#include "QList"
#include "QString"

namespace Lua {

class LuaEdge
{
public:
    LuaEdge();
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

#endif // LUAEDGE_H
