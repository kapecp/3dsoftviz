#include "LuaGraph/LuaGraph.h"
#include "LuaInterface/LuaInterface.h"
#include "Diluculum/LuaState.hpp"

Lua::LuaGraph::LuaGraph()
{
    nodes = new QMap<qlonglong, LuaNode*>();
    edges = new QMap<qlonglong, LuaEdge*>();
    incidences = new QMap<qlonglong, LuaIncidence*>();
}

Lua::LuaGraph* Lua::LuaGraph::loadGraph()
{
    Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();

    lua->executeFile("../share/3dsoftviz/scripts/main.lua");
    Lua::LuaGraph* result = new Lua::LuaGraph;

    Diluculum::LuaState ls = (*lua->getLuaState());
    Diluculum::LuaValueMap edges = ls["graph"].value().asTable();
    for(Diluculum::LuaValueMap::iterator iterator = edges.begin(); iterator != edges.end(); iterator++) {
        qlonglong id = iterator->first.asTable()["id"].asInteger();

        Lua::LuaEdge* edge = new Lua::LuaEdge();
        edge->setId(id);
        result->edges->insert(id, edge);

        Diluculum::LuaValueMap incidences = iterator->second.asTable();
        for(Diluculum::LuaValueMap::iterator iterator2 = incidences.begin(); iterator2 != incidences.end(); iterator2++) {
            qlonglong id2 = iterator2->first.asTable()["id"].asInteger();
            edge->addIncidence(id2);
            Lua::LuaIncidence* incidence = new Lua::LuaIncidence();
            incidence->setId(id2);

            qlonglong id3 = iterator2->second.asTable()["id"].asInteger();
            if (result->nodes->contains(id3)){
                result->nodes->value(id3)->addIncidence(id2);
            } else {
                Lua::LuaNode* node = new Lua::LuaNode();
                node->setId(id3);
                node->addIncidence(id2);
                result->nodes->insert(id3, node);
            }

            incidence->setEdgeNode(id, id3);
            result->incidences->insert(id2, incidence);
        }
    }

    cout << std::endl << "Nodes:" << std::endl;
    for (QMap<qlonglong, LuaNode*>::iterator i = result->nodes->begin(); i != result->nodes->end(); i++){
        cout << "N"<< (*i)->getId() << ":";
        for (int j = 0; j < (*i)->getIncidences().size(); j++)
            cout << " I" << (*i)->getIncidences()[j];
        cout << endl;
    }

    cout << std::endl << "Edges:" << std::endl;
    for (QMap<qlonglong, LuaEdge*>::iterator i = result->edges->begin(); i != result->edges->end(); i++){
        cout << "E"<< (*i)->getId() << ":";
        for (int j = 0; j < (*i)->getIncidences().size(); j++)
            cout << " I" << (*i)->getIncidences()[j];
        cout << endl;
    }

    cout << std::endl << "Incidences:" << std::endl;
    for (QMap<qlonglong, LuaIncidence*>::iterator i = result->incidences->begin(); i != result->incidences->end(); i++){
        cout << "I"<< (*i)->getId() << ":";
        QPair<qlonglong, qlonglong> inc = (*i)->getEdgeNodePair();
        cout << " E" << inc.first << " <-> N" << inc.second;
        cout << endl;
    }

    return result;
}

Lua::LuaGraph::~LuaGraph()
{
    for (QMap<qlonglong, LuaNode*>::iterator i = nodes->begin(); i != nodes->end(); i++){
        delete (*i);
    }

    for (QMap<qlonglong, LuaEdge*>::iterator i = edges->begin(); i != edges->end(); i++){
        delete (*i);
    }

    for (QMap<qlonglong, LuaIncidence*>::iterator i = incidences->begin(); i != incidences->end(); i++){
       delete (*i);
    }
    delete nodes;
    delete incidences;
    delete edges;
}
