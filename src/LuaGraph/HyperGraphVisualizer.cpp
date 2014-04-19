#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"

Lua::HyperGraphVisualizer::HyperGraphVisualizer(Data::Graph *graph)
    : Lua::LuaGraphVisualizer(graph){}

void Lua::HyperGraphVisualizer::visualize()
{
    Lua::LuaGraph *g = Lua::LuaGraph::loadGraph();
    g->printGraph();

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         setNodeParams(n, i.value(), osg::Vec4f(1,0,0,1), 8);
    }

    for (QMap<qlonglong, Lua::LuaEdge *>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i){
         osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
         setNodeParams(n, i.value(), osg::Vec4f(0,1,0,1), 8);
    }

    for (QMap<qlonglong, Lua::LuaIncidence *>::iterator i = g->getIncidences()->begin(); i != g->getIncidences()->end(); ++i){
        osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().first);
        osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value(i.value()->getEdgeNodePair().second);
        if (i.value()->getOriented()){
            if (i.value()->getOutGoing())
                currentGraph->addEdge(i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, true);
            else
                currentGraph->addEdge(i.key(), i.value()->getLabel(), dstNode, srcNode, edgeType, true);
        } else
            currentGraph->addEdge(i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, false);
    }
    g->setObserver(this);

    QString metaNodeName = "metaNode";
    QString metaEdgeName = "metaEdge";
    osg::ref_ptr<Data::Node> filesAnchor = currentGraph->addNode(metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3(0, 0, 1000));
    osg::ref_ptr<Data::Node> functionsAnchor = currentGraph->addNode(metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3(0, 0, -1000));

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i){
        if (i.value()->getParams()["root"]== true){
            std::cout << "connecting to files meta node ";
            osg::ref_ptr<Data::Edge> me = currentGraph->addEdge(metaEdgeName, currentGraph->getNodes()->value(i.value()->getId()), filesAnchor, currentGraph->getEdgeMetaType(), false);
            std::cout << me->getId() << std::endl;
        }
    }
}

void Lua::HyperGraphVisualizer::onUpdate()
{
    std::cout << "Graph update called" << std::endl;
}


