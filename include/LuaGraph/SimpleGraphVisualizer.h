#ifndef LUAGRAPHSIMPLEGRAPHVISUALIZER_H
#define LUAGRAPHSIMPLEGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

class SimpleGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
    SimpleGraphVisualizer(Data::Graph *graph);

    void visualize();

    void onUpdate();
};

}

#endif // LUAGRAPHSIMPLEGRAPHVISUALIZER_H
