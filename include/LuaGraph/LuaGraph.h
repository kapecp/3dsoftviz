#ifndef LUAGRAPH_H
#define LUAGRAPH_H

#include "QList"
#include "QString"
#include "Q_LONG"
#include "LuaGraph/LuaNode.h"
#include "LuaGraph/LuaEdge.h"
#include "LuaGraph/LuaIncidence.h"

namespace Lua {

class LuaGraph
{
public:
    LuaGraph();
    static LuaGraph *loadGraph();

    void visualize(Data::Graph *graph, bool incidence_as_node = false);

    ~LuaGraph();

    QMap<qlonglong, LuaNode *> *getNodes() const;

    QMap<qlonglong, LuaEdge *> *getEdges() const;

    QMap<qlonglong, LuaIncidence *> *getIncidences() const;

private:
    QMap<qlonglong, LuaNode *> *nodes;
    QMap<qlonglong, LuaEdge *> *edges;
    QMap<qlonglong, LuaIncidence *> *incidences;
};

}

#endif // LUAGRAPH_H
