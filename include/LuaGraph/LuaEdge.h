#ifndef LUAEDGE_H
#define LUAEDGE_H

#include "QList"
#include "Q_LONG"
#include "QString"

namespace Lua {

class LuaEdge
{
public:
    LuaEdge();
    qlonglong getId() const;
    void setId(qlonglong value);

    QString getLabel() const;
    void setLabel(const QString &value);

    void addIncidence(qlonglong id);

    QList<qlonglong> getIncidences() const;

private:
    qlonglong id;
    QString label;
    QList<qlonglong> incidences;
};

}

#endif // LUAEDGE_H
