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

};

}

#endif // LUAGRAPHVISUALIZER_H
