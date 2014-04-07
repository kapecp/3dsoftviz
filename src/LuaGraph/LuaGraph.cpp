#include "LuaGraph/LuaGraph.h"
#include "LuaInterface/LuaInterface.h"
#include "Diluculum/LuaState.hpp"

#include "Data/Graph.h"
#include "Importer/GraphOperations.h"
#include <Diluculum/LuaWrappers.hpp>

Lua::LuaGraph *Lua::LuaGraph::instance;

Diluculum::LuaValueList luaCallback (const Diluculum::LuaValueList& params)
{
    std::cout << "C callback" << std::endl;
    if (!Lua::LuaGraph::hasObserver()) return Diluculum::LuaValueList();
    Lua::LuaGraph::getInstance()->getObserver()->onUpdate();
    return Diluculum::LuaValueList();
}
DILUCULUM_WRAP_FUNCTION(luaCallback)

Lua::LuaGraph::LuaGraph()
{
    nodes = new QMap<qlonglong, Lua::LuaNode*>();
    edges = new QMap<qlonglong, Lua::LuaEdge*>();
    incidences = new QMap<qlonglong, Lua::LuaIncidence*>();
    observer = NULL;
    Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();
    lua->executeFile("../share/3dsoftviz/scripts/main.lua");
    Diluculum::LuaState *ls = lua->getLuaState();
    (*ls)["callback"] = DILUCULUM_WRAPPER_FUNCTION (luaCallback);
}
Lua::LuaGraphObserver *Lua::LuaGraph::getObserver() const
{
    return observer;
}

void Lua::LuaGraph::setObserver(Lua::LuaGraphObserver *value)
{
    observer = value;
}

void Lua::LuaGraph::clearGraph()
{
    nodes->clear();
    edges->clear();
    incidences->clear();
}

void Lua::LuaGraph::printGraph()
{
    std::cout << "Incidences " << incidences->count() << std::endl;
    for (QMap<qlonglong, LuaIncidence*>::iterator i = incidences->begin(); i != incidences->end(); i++) {
        std::cout << i.key() << " ";
    }
    std::cout << std::endl;
    std::cout << "Nodes " << nodes->count() << std::endl;
    for (QMap<qlonglong, LuaNode*>::iterator i = nodes->begin(); i != nodes->end(); i++) {
        std::cout << i.key() << " ";
    }
    std::cout << std::endl;
    std::cout << "Edges " << edges->count() << std::endl;
    for (QMap<qlonglong, LuaEdge*>::iterator i = edges->begin(); i != edges->end(); i++) {
        std::cout << i.key() << " ";
    }
    std::cout << std::endl;
}

Lua::LuaGraph *Lua::LuaGraph::getInstance()
{
    if (instance == NULL) instance = new Lua::LuaGraph();
    return instance;
}

Lua::LuaGraph *Lua::LuaGraph::loadGraph()
{
    Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();

    Lua::LuaGraph* result = Lua::LuaGraph::getInstance();
    result->clearGraph();

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

    std::cout << "Node count: " << result->nodes->count() << std::endl;
    return result;
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

bool Lua::LuaGraph::hasObserver()
{
    if (instance == NULL) return false;
    return instance->observer != NULL;
}

QMap<qlonglong, Lua::LuaEdge *> *Lua::LuaGraph::getEdges() const
{
    return edges;
}

QMap<qlonglong, Lua::LuaNode *> *Lua::LuaGraph::getNodes() const
{
    return nodes;
}

