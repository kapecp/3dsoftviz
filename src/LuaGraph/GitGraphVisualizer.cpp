#include "LuaGraph/GitGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"
#include "LuaGraph/LuaIncidence.h"

#include "Data/Graph.h"

#include "Manager/Manager.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitVersion.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"
#include "GitLib/GitUtils.h"
#include "GitLib/GitMetaData.h"

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

    QList<QString> changedNodes = Repository::Git::GitUtils::getModifiedLuaNodesFromVersion( evolutionGraph, currentGraph->getCurrentVersion() );

    //    qDebug() << "Zaciatok vykreslovania GitLuaGraphu";
    for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
        // ziskame referenciu na existujuci uzol v grafe alebo NULL

        osg::ref_ptr<Data::Node> n = currentGraph->findNodeByLuaIdentifier( i.value()->getIdentifier() );

        // ak sa uzol v grafe nenachadza, tak ho pridame do grafu a nastavime mu LuaIdentifier
        if( !n ) {
            n = currentGraph->addNode( i.key() , i.value()->getLabel(), nodeType );
            n->setLuaIdentifier( i.value()->getIdentifier() );
        }

        n->setType( nodeType );
        setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
//        evolutionGraph->addLuaNodesMapping( i.value()->getIdentifier(), i.value()->getId() );
        evolutionGraph->getMetaDataFromIdentifier( i.value()->getIdentifier() )->setLuaMapping( i.value()->getId() );
    }

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
        newEdge->setEdgeColor( osg::Vec4f( 1,1,1,1 ) );
//        evolutionGraph->addLuaEdgesMapping( i.value()->getIdentifier(), i.value()->getId() );
        evolutionGraph->getMetaDataFromIdentifier( i.value()->getIdentifier() )->setLuaMapping( i.value()->getId() );
    }
    g->setObserver( this );
    /*
    QString metaNodeName = "metaNode";
    QString metaEdgeName = "metaEdge";
    osg::ref_ptr<Data::Node> filesAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max(), metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, 500 ) );
    osg::ref_ptr<Data::Node> functionsAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max() - 1, metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, -500 ) );
    filesAnchor->setColor( osg::Vec4( 0,0,0,0 ) );
    functionsAnchor->setColor( osg::Vec4( 0,0,0,0 ) );

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

    setUpGraphTypes();

    qDebug() << "From git graph visualizer";
    foreach( QString string, changedNodes ) {
        QStringList list = string.split( ";" );
//        qDebug() << list.at( 1 );
        osg::ref_ptr<Data::Node> node = currentGraph->findNodeByLuaIdentifier( list.at( 1 ) );
        if( node ) {
//            qDebug() << "Set up removed Node" << node->getLuaIdentifier();
            if( !QString::compare( "ADDED", list.at( 0 ) ) ) {
                node->setType( currentGraph->getTypesByName( "addedNode" ).at( 0 ) );
            } else {
                node->setType( currentGraph->getTypesByName( "removedNode" ).at( 0 ) );
            }
            node->reloadConfig();
            node->showLabel( true );
        }

        osg::ref_ptr<Data::Edge> edge = currentGraph->findEdgeByLuaIdentifier( list.at( 2 ) + "+" + list.at( 1 ) );
        if( edge ) {

            if( !QString::compare( "ADDED", list.at( 0 ) ) ) {
                edge->setEdgeColor( osg::Vec4f( 0,1,0,1 ) );
            } else {
                edge->setEdgeColor( osg::Vec4f( 1,0,0,1 ) );
            }

        }
    }
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

void Lua::GitGraphVisualizer::setUpGraphTypes() {
    QMap<QString, QString>* settings = new QMap<QString, QString>;
    settings->insert( "color.R", "1" );
    settings->insert( "color.G", "1" );
    settings->insert( "color.B", "1" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.AddedNode" ) );;
    this->currentGraph->addType( "addedNode", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "1" );
    settings->insert( "color.G", "1" );
    settings->insert( "color.B", "1" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.RemoveNode" ) );;
    this->currentGraph->addType( "removedNode", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "1" );
    settings->insert( "color.G", "1" );
    settings->insert( "color.B", "1" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.AuthorNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Author" ) );
    this->currentGraph->addType( "author", settings );

    settings = new QMap<QString, QString>;
    settings->insert( "color.R", "0" );
    settings->insert( "color.G", "0" );
    settings->insert( "color.B", "1" );
    settings->insert( "color.A", "1" );
    settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
    settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.OrientedEdge" ) );
    this->currentGraph->addType( "authorEdge", settings );
}


