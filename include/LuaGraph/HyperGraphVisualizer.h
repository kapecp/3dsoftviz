#ifndef HYPERGRAPHVISUALIZER_H
#define HYPERGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

class HyperGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
    HyperGraphVisualizer(Data::Graph *graph, osg::ref_ptr<osg::Camera> camera = NULL);

    void visualize();

    void onUpdate();
};

}

#endif // HYPERGRAPHVISUALIZER_H
