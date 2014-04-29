#include "LuaGraph/LuaGraphVisualizer.h"
#include "Importer/GraphOperations.h"

Lua::LuaGraphVisualizer::LuaGraphVisualizer(Data::Graph *graph, osg::ref_ptr<osg::Camera> camera){
    currentGraph = graph;
    this->camera = camera;
    Importer::GraphOperations *operations = new Importer::GraphOperations(*currentGraph);
    operations->addDefaultTypes(edgeType, nodeType);
}

void Lua::LuaGraphVisualizer::setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj, osg::Vec4f defColor, float defSize){
    node->setName(obj->getLabel());
    node->setLabelText(obj->getLabel());
    float r = obj->getFloatParam("colorR", defColor.r());
    float g = obj->getFloatParam("colorG", defColor.g());
    float b = obj->getFloatParam("colorB", defColor.b());
    float a = obj->getFloatParam("colorA", defColor.a());
    node.get()->setColor(osg::Vec4 (r,g,b,a));
    float scale = obj->getFloatParam("size", defSize);
    node->setScale(scale);
    node->reloadConfig();
}
