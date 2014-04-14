#ifndef FULLHYPERGRAPHVISUALIZER_H
#define FULLHYPERGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

class FullHyperGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
    FullHyperGraphVisualizer(Data::Graph *graph);

    void visualize();

    void onUpdate();

private:
    Data::Graph *currentGraph;
    Data::Type *edgeType = NULL;
    Data::Type *nodeType = NULL;
    void setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj);
};

}


#endif // FULLHYPERGRAPHVISUALIZER_H
