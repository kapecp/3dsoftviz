#ifndef LUAGRAPH_H
#define LUAGRAPH_H

#include "QList"
#include "QString"
#include "Q_LONG"
#include "LuaGraph/LuaNode.h"
#include "LuaGraph/LuaEdge.h"
#include "LuaGraph/LuaIncidence.h"
#include "LuaGraph/LuaGraphObserver.h"

namespace Lua {

class LuaGraph
{
public:
    static LuaGraph *getInstance();
    static LuaGraph *loadGraph();

    void visualize(Data::Graph *graph, bool incidence_as_node = false);

    ~LuaGraph();

    QMap<qlonglong, LuaNode *> *getNodes() const;

    QMap<qlonglong, LuaEdge *> *getEdges() const;

    QMap<qlonglong, LuaIncidence *> *getIncidences() const;

    static bool hasObserver();
    Lua::LuaGraphObserver *getObserver() const;
    void setObserver(Lua::LuaGraphObserver *value);

    void clearGraph();

    void printGraph();
private:
    LuaGraph();

    static LuaGraph *instance;
    Lua::LuaGraphObserver *observer;

    QMap<qlonglong, LuaNode *> *nodes;
    QMap<qlonglong, LuaEdge *> *edges;
    QMap<qlonglong, LuaIncidence *> *incidences;


};

}

#endif // LUAGRAPH_H
