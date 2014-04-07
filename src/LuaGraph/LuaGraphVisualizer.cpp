#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"

Lua::LuaGraphVisualizer::LuaGraphVisualizer(Data::Graph *graph)
{
    currentGraph = graph;
}

void Lua::LuaGraphVisualizer::visualize(bool incidence_as_node)
{
    Lua::LuaGraph *g = Lua::LuaGraph::loadGraph();

    Data::Type *edgeType = NULL;
    Data::Type *nodeType = NULL;
    Importer::GraphOperations *operations = new Importer::GraphOperations(*currentGraph);
    operations->addDefaultTypes(edgeType, nodeType);

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         n.get()->setColor(osg::Vec4 (1,0,0,1));
         n.get()->reloadConfig();
    }

    for (QMap<qlonglong, Lua::LuaEdge *>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         n.get()->setColor(osg::Vec4 (0,0,1,1));
         n.get()->reloadConfig();
    }

    if (incidence_as_node){
        for (QMap<qlonglong, Lua::LuaIncidence *>::iterator i = g->getIncidences()->begin(); i != g->getIncidences()->end(); ++i){
            osg::ref_ptr<Data::Node> incNode = currentGraph->addNode(i.key(), i.value()->getLabel(), nodeType);
            incNode.get()->setColor(osg::Vec4 (0,1,0,1));
            incNode.get()->reloadConfig();

            osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().first);
            currentGraph->addEdge(i.value()->getLabel(), srcNode, incNode, edgeType, false);

            osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().second);
            currentGraph->addEdge(i.value()->getLabel(), incNode, dstNode, edgeType, false);
        }
    } else {
        for (QMap<qlonglong, Lua::LuaIncidence *>::iterator i = g->getIncidences()->begin(); i != g->getIncidences()->end(); ++i){
            osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().first);
            osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().second);
            currentGraph->addEdge(i.value()->getLabel(), srcNode, dstNode, edgeType, false);
        }
    }
    g->setObserver(this);
}

void Lua::LuaGraphVisualizer::onUpdate()
{
    cout << "Graph update called" << std::endl;
}
