#ifndef LUAGRAPHSIMPLEGRAPHVISUALIZER_H
#define LUAGRAPHSIMPLEGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

class SimpleGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
    SimpleGraphVisualizer(Data::Graph *graph, osg::ref_ptr<osg::Camera> camera = 0);

    void visualize();

    void onUpdate();
};

}

#endif // LUAGRAPHSIMPLEGRAPHVISUALIZER_H
