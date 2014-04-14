#include "LuaGraph/SimpleGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"

Lua::SimpleGraphVisualizer::SimpleGraphVisualizer(Data::Graph *graph)
{
    currentGraph = graph;
    Importer::GraphOperations *operations = new Importer::GraphOperations(*currentGraph);
    operations->addDefaultTypes(edgeType, nodeType);
}

void Lua::SimpleGraphVisualizer::visualize()
{
    Lua::LuaGraph *g = Lua::LuaGraph::loadGraph();
    g->printGraph();

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         setNodeParams(n, i.value());
    }

    for (QMap<qlonglong, Lua::LuaEdge *>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i){
        if (i.value()->getIncidences().size() != 2) throw new std::runtime_error("Not a simple graph");
        LuaIncidence * const incid1 = g->getIncidences()->value(i.value()->getIncidences()[0]);
        LuaIncidence * const incid2 = g->getIncidences()->value(i.value()->getIncidences()[1]);
        osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value(incid1->getEdgeNodePair().second);
        osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value(incid2->getEdgeNodePair().second);
        if (incid1->getOriented()){
            if (incid1->getOutGoing()){
                currentGraph->addEdge(i.key(), i.value()->getLabel(), dstNode, srcNode, edgeType, true);
            } else {
                currentGraph->addEdge(i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, true);
            }
        } else {
            currentGraph->addEdge(i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, false);
        }
    }
    g->setObserver(this);
}

void Lua::SimpleGraphVisualizer::setNodeParams(osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject *obj){
    float r = obj->getFloatParam("colorR", 1);
    float g = obj->getFloatParam("colorG", 1);
    float b = obj->getFloatParam("colorB", 1);
    float a = obj->getFloatParam("colorA", 1);
    node.get()->setColor(osg::Vec4 (r,g,b,a));
    float scale = obj->getFloatParam("size", 8);
    node->setScale(scale);
    node->reloadConfig();
}

void Lua::SimpleGraphVisualizer::onUpdate()
{
    std::cout << "Graph update called" << std::endl;
}



