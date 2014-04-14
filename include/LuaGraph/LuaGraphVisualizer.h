#ifndef LUAGRAPHVISUALIZER_H
#define LUAGRAPHVISUALIZER_H

#include "LuaGraph/LuaGraphObserver.h"
#include "Data/Graph.h"

namespace Lua {

class LuaGraphVisualizer : public Lua::LuaGraphObserver
{
public:
    LuaGraphVisualizer(Data::Graph *graph);

    void visualize(bool incidence_as_node = false);

    // LuaGraphObserver interface
    void onUpdate();
private:
    Data::Graph *currentGraph;
    Data::Type *edgeType = NULL;
    Data::Type *nodeType = NULL;
    void setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj);
};

}

#endif // LUAGRAPHVISUALIZER_H
