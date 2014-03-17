#ifndef LUANODE_H
#define LUANODE_H

#include "QList"
#include "Q_LONG"
#include "QString"

namespace Lua {

class LuaNode
{
public:
    LuaNode();
    qlonglong getId() const;
    void setId(qlonglong value);

    QString getLabel() const;
    void setLabel(const QString &value);

    void addIncidence(const qlonglong id);

    QList<qlonglong> getIncidences() const;

private:
    qlonglong id;
    QString label;
    QList<qlonglong> incidences;

};

}

#endif // LUANODE_H
