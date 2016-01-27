#include "LuaGraph/GitGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"

#include "Data/Graph.h"

#include "Manager/Manager.h"
#include "GitLib/GitEvolutionGraph.h"

#include <QMap>

#include "limits"
#include <iostream>

#include <QDebug>

Lua::GitGraphVisualizer::GitGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera )
	: Lua::LuaGraphVisualizer( graph, camera ) {}

void Lua::GitGraphVisualizer::visualize()
{
    Repository::Git::GitEvolutionGraph* evolutionGraph =  Manager::GraphManager::getInstance()->getActiveEvolutionGraph();

    Lua::LuaGraph* g = Lua::LuaGraph::loadEvoGraph( evolutionGraph->getFilePath() );
//    g->printGraph();


//    qDebug() << "Zaciatok vykreslovania GitLuaGraphu";
    for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
        osg::ref_ptr<Data::Node> n = currentGraph->findNodeByLuaIdentifier( i.value()->getIdentifier() );
        if( !n ) {
            n = currentGraph->addNode( i.key() , i.value()->getLabel(), nodeType );
            n->setLuaIdentifier( i.value()->getIdentifier() );
        }
        setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
        evolutionGraph->addLuaNodesMapping( i.value()->getIdentifier(), i.value()->getId() );
    }

    qDebug() << "Koniec spracovania uzlov";

    for ( QMap<qlonglong, Lua::LuaEdge*>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i ) {
        Lua::LuaEdge* edge = i.value();
        QString identifier = edge->getIdentifier();
//        qDebug() << identifier;
        if ( i.value()->getIncidences().size() != 2 ) {
            throw new std::runtime_error( "Not a simple graph" );
        }
        LuaIncidence* const incid1 = g->getIncidences()->value( i.value()->getIncidences()[0] );
        LuaIncidence* const incid2 = g->getIncidences()->value( i.value()->getIncidences()[1] );
        osg::ref_ptr<Data::Node> srcNode = currentGraph->findNodeByLuaIdentifier( g->getNodes()->find( incid1->getEdgeNodePair().second ).value()->getIdentifier() );
        osg::ref_ptr<Data::Node> dstNode = currentGraph->findNodeByLuaIdentifier( g->getNodes()->find( incid2->getEdgeNodePair().second ).value()->getIdentifier() );
        osg::ref_ptr<Data::Edge> newEdge = currentGraph->findEdgeByLuaIdentifier( identifier );
        if( !newEdge ) {
            if ( incid1->getOriented() ) {
                if ( incid1->getOutGoing() ) {
                    newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), dstNode, srcNode, edgeType, true );
                }
                else {
                    newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, true );
                }
            }
            else {
                newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, false );
            }
            newEdge->setLuaIdentifier( identifier );
        }

        newEdge->setCamera( camera );
        setEdgeParams( newEdge, i.value(), osg::Vec4f( 1,1,1,1 ) );
        evolutionGraph->addLuaEdgesMapping( i.value()->getIdentifier(), i.value()->getId() );
    }
    g->setObserver( this );

    QString metaNodeName = "metaNode";
    QString metaEdgeName = "metaEdge";
    osg::ref_ptr<Data::Node> filesAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max(), metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, 500 ) );
    osg::ref_ptr<Data::Node> functionsAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max() - 1, metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, -500 ) );
    filesAnchor->setColor( osg::Vec4( 0,0,0,0 ) );
    functionsAnchor->setColor( osg::Vec4( 0,0,0,0 ) );
/*
    for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
        if ( i.value()->getParams().type() == 0 ) {
            continue;
        }
        if ( i.value()->getParams()["root"]== true ) {
            osg::ref_ptr<Data::Node> root = currentGraph->getNodes()->value( i.key() );
            osg::ref_ptr<Data::Edge> metaLink = currentGraph->addEdge( metaEdgeName, root, filesAnchor, currentGraph->getEdgeMetaType(), false );
            metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
            metaLink->setInvisible( true );
        }
        if ( i.value()->getParams()["type"] == "function" ) {
            osg::ref_ptr<Data::Node> func = currentGraph->getNodes()->value( i.key() );
            osg::ref_ptr<Data::Edge> metaLink = currentGraph->addEdge( metaEdgeName, func, functionsAnchor, currentGraph->getEdgeMetaType(), false );
            metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
            metaLink->setInvisible( true );
            metaLink->setEdgeStrength( 0.1f );
        }
    }*/
}

void Lua::GitGraphVisualizer::onUpdate()
{
	std::cout << "SimpleGraph update called" << std::endl;
	Lua::LuaGraph* g = Lua::LuaGraph::loadGraph();
	for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
		osg::ref_ptr<Data::Node> n = currentGraph->getNodes()->value( i.key() );
		setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
	}
	for ( QMap<qlonglong, Lua::LuaEdge*>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i ) {
		osg::ref_ptr<Data::Edge> e = currentGraph->getEdges()->value( i.key() );
		setEdgeParams( e, i.value(), osg::Vec4f( 1,1,1,1 ) );
		e->reloadLabel();
	}
}



