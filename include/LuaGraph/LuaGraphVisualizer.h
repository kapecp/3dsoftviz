#ifndef LUAGRAPHVISUALIZER_H
#define LUAGRAPHVISUALIZER_H

#include "LuaGraph/LuaGraphObserver.h"
#include "LuaGraph/LuaGraphObject.h"
#include "Data/Graph.h"

namespace Lua {

class LuaGraphVisualizer : public Lua::LuaGraphObserver
{
public:
    LuaGraphVisualizer(Data::Graph *graph, osg::ref_ptr<osg::Camera> camera = 0);
    virtual void visualize() = 0;
protected:
    Data::Graph *currentGraph;
    osg::ref_ptr<osg::Camera> camera;
    Data::Type *edgeType = NULL;
    Data::Type *nodeType = NULL;
    void setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj, osg::Vec4f defColor, float defSize);
};

}

#endif // LUAGRAPHVISUALIZER_H
