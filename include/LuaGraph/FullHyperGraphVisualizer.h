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
};

}


#endif // FULLHYPERGRAPHVISUALIZER_H
