#ifndef LUAGRAPHVISUALIZER_H
#define LUAGRAPHVISUALIZER_H

#include "LuaGraph/LuaGraphObserver.h"
#include "Data/Graph.h"

namespace Lua {

class LuaGraphVisualizer : public Lua::LuaGraphObserver
{
public:
    virtual void visualize() = 0;
};

}

#endif // LUAGRAPHVISUALIZER_H
