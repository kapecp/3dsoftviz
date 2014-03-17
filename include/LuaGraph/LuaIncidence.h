#ifndef LUAINCIDENCE_H
#define LUAINCIDENCE_H

#include "QPair"
#include "Q_LONG"
#include "QString"

namespace Lua {

class LuaIncidence
{
public:
    LuaIncidence();
    qlonglong getId() const;
    void setId(qlonglong value);

    QString getLabel() const;
    void setLabel(const QString &value);

    void setEdgeNode(const qlonglong edgeId, const qlonglong nodeId);

    QPair<qlonglong, qlonglong> getEdgeNodePair() const;

private:
    qlonglong id;
    QString label;
    QPair<qlonglong, qlonglong> edgeNodePair;
};

}

#endif // LUAINCIDENCE_H
