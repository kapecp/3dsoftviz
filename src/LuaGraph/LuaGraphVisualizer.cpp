#include "LuaGraph/LuaGraphVisualizer.h"

Lua::LuaGraphVisualizer::LuaGraphVisualizer(Data::Graph *graph){
    currentGraph = graph;
    Importer::GraphOperations *operations = new Importer::GraphOperations(*currentGraph);
    operations->addDefaultTypes(edgeType, nodeType);
}

void Lua::LuaGraphVisualizer::setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj, osg::Vec4f defColor, float defSize){
    float r = obj->getFloatParam("colorR", defColor.r());
    float g = obj->getFloatParam("colorG", defColor.g());
    float b = obj->getFloatParam("colorB", defColor.b());
    float a = obj->getFloatParam("colorA", defColor.a());
    node.get()->setColor(osg::Vec4 (r,g,b,a));
    float scale = obj->getFloatParam("size", defSize);
    node->setScale(scale);
    node->reloadConfig();
}
