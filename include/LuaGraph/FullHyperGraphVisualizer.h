#ifndef FULLHYPERGRAPHVISUALIZER_H
#define FULLHYPERGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

class FullHyperGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
    FullHyperGraphVisualizer(Data::Graph *graph, osg::Camera *camera = NULL);

    void visualize();

    void onUpdate();
};

}


#endif // FULLHYPERGRAPHVISUALIZER_H
