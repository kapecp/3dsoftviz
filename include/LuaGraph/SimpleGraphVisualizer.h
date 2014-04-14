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

private:
    Data::Graph *currentGraph;
    Data::Type *edgeType = NULL;
    Data::Type *nodeType = NULL;
    void setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj);
};

}

#endif // LUAGRAPHSIMPLEGRAPHVISUALIZER_H
