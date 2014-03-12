#ifndef LUAGRAPH_H
#define LUAGRAPH_H

#include "QList"
#include "QString"
#include "LuaGraph/LuaNode.h"
#include "LuaGraph/LuaEdge.h"
#include "LuaGraph/LuaIncidence.h"

namespace Lua {

class LuaGraph
{
public:
    LuaGraph();

private:
    QList<LuaNode> nodes;
    QList<LuaEdge> edges;
    QList<LuaIncidence> incidences;
};

}

#endif // LUAGRAPH_H
