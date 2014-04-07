#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"

Lua::LuaGraphVisualizer::LuaGraphVisualizer(Data::Graph *graph)
{
    currentGraph = graph;
    Importer::GraphOperations *operations = new Importer::GraphOperations(*currentGraph);
    operations->addDefaultTypes(edgeType, nodeType);
}

void Lua::LuaGraphVisualizer::visualize(bool incidence_as_node)
{
    Lua::LuaGraph *g = Lua::LuaGraph::loadGraph();
    g->printGraph();

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
            currentGraph->addEdge(i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, false);
        }
    }
    g->setObserver(this);
}

void Lua::LuaGraphVisualizer::onUpdate()
{
    cout << "Graph update called" << std::endl;

    Lua::LuaGraph *g = Lua::LuaGraph::loadGraph();
    g->printGraph();
//    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = g->getNodes()->begin(); i!= g->getNodes()->end(); i++){
//        if (!currentGraph->getNodes()->contains(i.key())){
//            cout << "Adding node " << i.key() << std::endl;
//            osg::ref_ptr<Data::Node> n = currentGraph->addNode(i.key() , i.value()->getLabel(), nodeType);
//            n.get()->setColor(osg::Vec4 (1,0,0,1));
//            n.get()->reloadConfig();
//        }
//    }
    for (QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = currentGraph->getEdges()->begin(); i != currentGraph->getEdges()->end(); i++){
        if (!g->getIncidences()->contains(i.key())){
            cout << "Removing edge " << i.key() << std::endl;
            currentGraph->removeEdge(i.value());
        }
    }
//    for (QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator i = currentGraph->getNodes()->begin(); i != currentGraph->getNodes()->end(); i++){
//        if (!g->getNodes()->contains(i.key()) && !g->getEdges()->contains(i.key()) && !g->getIncidences()->contains(i.key())){
//            cout << "Removing node " << i.key() << std::endl;
//            currentGraph->removeNode(i.value());
//        }
//    }
//    osg::ref_ptr<Data::Node> n = (*currentGraph->getNodes())[8];
//    n.get()->setColor(osg::Vec4(1,1,1,1));
//    currentGraph->removeNode(n);
//    n->releaseGLObjects();
}
