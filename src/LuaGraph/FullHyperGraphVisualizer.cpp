#include "LuaGraph/FullHyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"

Lua::FullHyperGraphVisualizer::FullHyperGraphVisualizer(Data::Graph *graph)
{
    currentGraph = graph;
    Importer::GraphOperations *operations = new Importer::GraphOperations(*currentGraph);
    operations->addDefaultTypes(edgeType, nodeType);
}

void Lua::FullHyperGraphVisualizer::visualize()
{
    Lua::LuaGraph *g = Lua::LuaGraph::loadGraph();
    g->printGraph();

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         setNodeParams(n, i.value());
    }

    for (QMap<qlonglong, Lua::LuaEdge *>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         setNodeParams(n, i.value());
    }

    for (QMap<qlonglong, Lua::LuaIncidence *>::iterator i = g->getIncidences()->begin(); i != g->getIncidences()->end(); ++i){
        osg::ref_ptr<Data::Node> incNode = currentGraph->addNode(i.key(), i.value()->getLabel(), nodeType);
        setNodeParams(incNode, i.value());

        osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().first);
        osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().second);
        if (i.value()->getOriented()){
            if (i.value()->getOutGoing()){
                currentGraph->addEdge(i.value()->getLabel(), srcNode, incNode, edgeType, true);
                currentGraph->addEdge(i.value()->getLabel(), incNode, dstNode, edgeType, true);
            } else {
                currentGraph->addEdge(i.value()->getLabel(), dstNode, incNode, edgeType, true);
                currentGraph->addEdge(i.value()->getLabel(), incNode, srcNode, edgeType, true);
            }
        } else {
            currentGraph->addEdge(i.value()->getLabel(), srcNode, incNode, edgeType, false);
            currentGraph->addEdge(i.value()->getLabel(), incNode, dstNode, edgeType, false);
        }
    }
    g->setObserver(this);
}

void Lua::FullHyperGraphVisualizer::setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj){
    float r = obj->getFloatParam("colorR", 1);
    float g = obj->getFloatParam("colorG", 1);
    float b = obj->getFloatParam("colorB", 1);
    float a = obj->getFloatParam("colorA", 1);
    node.get()->setColor(osg::Vec4 (r,g,b,a));
    float scale = obj->getFloatParam("size", 8);
    node->setScale(scale);
    node->reloadConfig();
}

void Lua::FullHyperGraphVisualizer::onUpdate()
{
    std::cout << "Graph update called" << std::endl;
}

