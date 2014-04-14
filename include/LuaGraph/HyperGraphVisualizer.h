#ifndef HYPERGRAPHVISUALIZER_H
#define HYPERGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

class HyperGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
    HyperGraphVisualizer(Data::Graph *graph);

    void visualize();

    void onUpdate();

private:
    Data::Graph *currentGraph;
    Data::Type *edgeType = NULL;
    Data::Type *nodeType = NULL;
    void setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj);
};

}

#endif // HYPERGRAPHVISUALIZER_H
