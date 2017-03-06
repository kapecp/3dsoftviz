#include "LuaGraph/ModuleGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"
#include <Manager/ResourceManager.h>

#include "ModuleGraph/Module.h"

#include "Data/Graph.h"
#include "Data/GraphLayout.h"

#include <QMap>
#include <QDebug>

#include "limits"
#include <iostream>

Lua::ModuleGraphVisualizer::ModuleGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera )
	: Lua::LuaGraphVisualizer( graph, camera ) {}

void Lua::ModuleGraphVisualizer::visualize()
{
	Lua::LuaGraph* g = Lua::LuaGraph::loadGraph();
	g->printGraph();

	auto resMgr = Manager::ResourceManager::getInstance();
	auto white = resMgr->getMaterial( osg::Vec3( 1.0f, 1.0f, 1.0f ) );

	for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
		osg::ref_ptr<Data::Node> n = currentGraph->addNode( i.key() , i.value()->getLabel(), nodeType );
		setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
	}

	for ( QMap<qlonglong, Lua::LuaEdge*>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i ) {
		if ( i.value()->getIncidences().size() != 2 ) {
			throw new std::runtime_error( "Not a simple graph" );
		}
		LuaIncidence* const incid1 = g->getIncidences()->value( i.value()->getIncidences()[0] );
		LuaIncidence* const incid2 = g->getIncidences()->value( i.value()->getIncidences()[1] );

		osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value( incid1->getEdgeNodePair().second );
		osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value( incid2->getEdgeNodePair().second );
		osg::ref_ptr<Data::Edge> newEdge;
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
		newEdge->setCamera( camera );
		setEdgeParams( newEdge, i.value(), osg::Vec4f( 1,1,1,1 ) );
	}
	g->setObserver( this );

	QString metaNodeName = "metaNode";
	QString metaEdgeName = "metaEdge";
	osg::ref_ptr<Data::Node> filesAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max(), metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, 500 ) );
	osg::ref_ptr<Data::Node> functionsAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max() - 1, metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, -500 ) );
	filesAnchor->setColor( osg::Vec4( 0,0,0,0 ) );
	functionsAnchor->setColor( osg::Vec4( 0,0,0,0 ) );

	for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
		if ( i.value()->getParams().getValue().type() == 0 ) {
			continue;
		}
		if ( i.value()->getParams().getValue()["root"] == true ) {
			osg::ref_ptr<Data::Node> root = currentGraph->getNodes()->value( i.key() );
			osg::ref_ptr<Data::Edge> metaLink = currentGraph->addEdge( metaEdgeName, root, filesAnchor, currentGraph->getEdgeMetaType(), false );
			metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
			metaLink->setInvisible( true );
		}
		if ( i.value()->getParams().getValue()["type"] == "function" ) {
			osg::ref_ptr<Data::Node> func = currentGraph->getNodes()->value( i.key() );
			osg::ref_ptr<Data::Edge> metaLink = currentGraph->addEdge( metaEdgeName, func, functionsAnchor, currentGraph->getEdgeMetaType(), false );
			metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
			metaLink->setInvisible( true );
			metaLink->setEdgeStrength( 0.1f );
		}
		if ( i.value()->getParams().getValue()["type"] == "directory" ) {
			osg::ref_ptr<Data::Node> fileNode = currentGraph->getNodes()->value( i.key() );

			auto module = new ModuleGraph::Module();
			auto ss = new osg::StateSet();
			ss->setAttribute( white );
			module->setStateSet( ss );
			module->refresh();
			fileNode->setModule( module );
		}
		if ( i.value()->getParams().getValue()["type"] == "file" ) {
			osg::ref_ptr<Data::Node> fileNode = currentGraph->getNodes()->value( i.key() );

			auto module = new ModuleGraph::Module();
			module->refresh();
			fileNode->setModule( module );
		}

	}
}

void Lua::ModuleGraphVisualizer::onUpdate()
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



