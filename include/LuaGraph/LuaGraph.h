#ifndef LUAGRAPH_H
#define LUAGRAPH_H

#include "LuaGraph/LuaNode.h"
#include "LuaGraph/LuaEdge.h"
#include "LuaGraph/LuaIncidence.h"

#include "Q_LONG"

namespace Lua {

class LuaGraphObserver;

class LuaGraph
{
public:
    static LuaGraph *getInstance();
    static LuaGraph *loadGraph();

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

Diluculum::LuaValueList luaCallback (const Diluculum::LuaValueList& params);

#endif // LUAGRAPH_H
