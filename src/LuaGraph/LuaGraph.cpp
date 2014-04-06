#include "LuaGraph/LuaGraph.h"
#include "LuaInterface/LuaInterface.h"
#include "Diluculum/LuaState.hpp"

#include "Data/Graph.h"
#include "Importer/GraphOperations.h"
#include <Diluculum/LuaWrappers.hpp>

Data::Graph *Lua::LuaGraph::currentGraph;
Lua::LuaGraph *Lua::LuaGraph::instance;

Lua::LuaGraph::LuaGraph()
{
    nodes = new QMap<qlonglong, Lua::LuaNode*>();
    edges = new QMap<qlonglong, Lua::LuaEdge*>();
    incidences = new QMap<qlonglong, Lua::LuaIncidence*>();
}

Diluculum::LuaValueList luaCallback (const Diluculum::LuaValueList& params)
{
    if (Lua::LuaGraph::currentGraph == NULL) return Diluculum::LuaValueList();
    cout << "C callback" << std::endl;
//    Lua::LuaGraph *graph = Lua::LuaGraph::loadGraph();

    QMap<qlonglong, osg::ref_ptr<Data::Node> > *nodes = Lua::LuaGraph::currentGraph->getNodes();
    osg::ref_ptr<Data::Node> node = (*nodes)[8];
    cout << "Scale: " << node->getScale() << std::endl;
    node->setScale(node->getScale() * 2);
    node->reloadConfig();
    cout << "Scale2: " << node->getScale() << std::endl;
    node->setColor(osg::Vec4 (1,0,0,1));

    return Diluculum::LuaValueList();
}
DILUCULUM_WRAP_FUNCTION(luaCallback)

Lua::LuaGraph *Lua::LuaGraph::getInstance()
{
    if (instance == NULL) instance = new Lua::LuaGraph();
    return instance;
}

Lua::LuaGraph *Lua::LuaGraph::loadGraph()
{
    cout << "Load graph called" << std::endl;

    Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();

    lua->executeFile("../share/3dsoftviz/scripts/main.lua");

    Lua::LuaGraph* result = Lua::LuaGraph::getInstance();

    Diluculum::LuaState *ls = lua->getLuaState();

    Diluculum::LuaValueMap edges = (*ls)["graph"].value().asTable();

    for(Diluculum::LuaValueMap::iterator iterator = edges.begin(); iterator != edges.end(); iterator++) {

        qlonglong id = iterator->first.asTable()["id"].asInteger();

        Lua::LuaEdge* edge = new Lua::LuaEdge();
        edge->setId(id);
        std::stringstream sstm1;
        sstm1 << "Edge " << id;
        edge->setLabel(QString::fromStdString(sstm1.str()));
        result->edges->insert(id, edge);


        Diluculum::LuaValueMap incidences = iterator->second.asTable();
        for(Diluculum::LuaValueMap::iterator iterator2 = incidences.begin(); iterator2 != incidences.end(); iterator2++) {
            qlonglong id2 = iterator2->first.asTable()["id"].asInteger();
            edge->addIncidence(id2);
            Lua::LuaIncidence* incidence = new Lua::LuaIncidence();
            incidence->setId(id2);
            std::stringstream sstm2;
            sstm2 << "Incid " << id2;
            incidence->setLabel(QString::fromStdString(sstm2.str()));

            qlonglong id3 = iterator2->second.asTable()["id"].asInteger();
            if (result->nodes->contains(id3)){
                result->nodes->value(id3)->addIncidence(id2);
            } else {
                Lua::LuaNode* node = new Lua::LuaNode();
                node->setId(id3);
                std::stringstream sstm;
                sstm << "Node " << id3;
                node->setLabel(QString::fromStdString(sstm.str()));
                node->addIncidence(id2);
                result->nodes->insert(id3, node);
            }

            incidence->setEdgeNode(id, id3);
            result->incidences->insert(id2, incidence);
        }
    }
    return result;
}

void Lua::LuaGraph::visualize(Data::Graph *graph, bool incidence_as_node)
{
    Data::Type *edgeType = NULL;
    Data::Type *nodeType = NULL;
    Importer::GraphOperations *operations = new Importer::GraphOperations(*graph);
    operations->addDefaultTypes(edgeType, nodeType);

    for (QMap<qlonglong, Lua::LuaNode *>::iterator i = getNodes()->begin(); i != getNodes()->end(); ++i){
         osg::ref_ptr<Data::Node> n = graph->addNode(i.key() , i.value()->getLabel(), nodeType);
         n.get()->setColor(osg::Vec4 (1,0,0,1));
         n.get()->reloadConfig();
    }

    for (QMap<qlonglong, Lua::LuaEdge *>::iterator i = getEdges()->begin(); i != getEdges()->end(); ++i){
         osg::ref_ptr<Data::Node> n = graph->addNode(i.key() , i.value()->getLabel(), nodeType);
         n.get()->setColor(osg::Vec4 (0,0,1,1));
         n.get()->reloadConfig();
    }

    if (incidence_as_node){
        for (QMap<qlonglong, Lua::LuaIncidence *>::iterator i = getIncidences()->begin(); i != getIncidences()->end(); ++i){
            osg::ref_ptr<Data::Node> incNode = graph->addNode(i.key(), i.value()->getLabel(), nodeType);
            incNode.get()->setColor(osg::Vec4 (0,1,0,1));
            incNode.get()->reloadConfig();

            osg::ref_ptr<Data::Node> srcNode = graph->getNodes()->value(i.value()->getEdgeNodePair().first);
            graph->addEdge(i.value()->getLabel(), srcNode, incNode, edgeType, false);

            osg::ref_ptr<Data::Node> dstNode = graph->getNodes()->value(i.value()->getEdgeNodePair().second);
            graph->addEdge(i.value()->getLabel(), incNode, dstNode, edgeType, false);
        }
    } else {
        for (QMap<qlonglong, Lua::LuaIncidence *>::iterator i = getIncidences()->begin(); i != getIncidences()->end(); ++i){
            osg::ref_ptr<Data::Node> srcNode = graph->getNodes()->value(i.value()->getEdgeNodePair().first);
            osg::ref_ptr<Data::Node> dstNode = graph->getNodes()->value(i.value()->getEdgeNodePair().second);
            graph->addEdge(i.value()->getLabel(), srcNode, dstNode, edgeType, false);
        }
    }
}

void Lua::LuaGraph::redisterUpdateCallback(Data::Graph *graph)
{
    Diluculum::LuaState *ls = LuaInterface::getInstance()->getLuaState();
    (*ls)["callback"] = DILUCULUM_WRAPPER_FUNCTION (luaCallback);
    currentGraph = graph;
}

Lua::LuaGraph::~LuaGraph()
{
    for (QMap<qlonglong, Lua::LuaNode*>::iterator i = nodes->begin(); i != nodes->end(); i++){
        delete (*i);
    }

    for (QMap<qlonglong, Lua::LuaEdge*>::iterator i = edges->begin(); i != edges->end(); i++){
        delete (*i);
    }

    for (QMap<qlonglong, Lua::LuaIncidence*>::iterator i = incidences->begin(); i != incidences->end(); i++){
       delete (*i);
    }
    delete nodes;
    delete incidences;
    delete edges;
}
QMap<qlonglong, Lua::LuaIncidence *> *Lua::LuaGraph::getIncidences() const
{
    return incidences;
}

QMap<qlonglong, Lua::LuaEdge *> *Lua::LuaGraph::getEdges() const
{
    return edges;
}

QMap<qlonglong, Lua::LuaNode *> *Lua::LuaGraph::getNodes() const
{
    return nodes;
}

