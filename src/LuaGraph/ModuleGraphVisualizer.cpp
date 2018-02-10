#include "LuaGraph/ModuleGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"
#include <Manager/ResourceManager.h>
#include "Manager/Manager.h"



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
	const float DEFAULT_FILE_BASE_SIZE = 5.0f;
	const float DEFAULT_FILE_HEIGHT = 5.0f;
	const float DEFAULT_NODE_BASE_SIZE = 4.0f;
	const float DEFAULT_NODE_HEIGHT = 4.0f;
	const float DEFAULT_NODE_MAX_HEIGHT = 10.0f;
	const float DEFAULT_BALL_SIZE = 1.0f;

	float maxFuncNodeSize = 0.0f;

	// insert every node and edge from Lua::LuaGraph to Data::Graph
	auto nodeType = currentGraph->addType( Data::GraphLayout::NESTED_NODE_TYPE );

	Lua::LuaGraph* luaGraph = Lua::LuaGraph::loadGraph();
	luaGraph->printGraph();

	// adding invisible anchor nodes
	QString metaNodeName = "metaNode";
	QString metaEdgeName = "metaEdge";
	auto filesAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max(), metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, 500 ) );
	auto functionsAnchor = currentGraph->addNode( std::numeric_limits<qlonglong>::max() - 1, metaNodeName, currentGraph->getNodeMetaType(), osg::Vec3( 0, 0, -500 ) );
	filesAnchor->setColor( osg::Vec4( 0,0,0,0 ) );
	functionsAnchor->setColor( osg::Vec4( 0,0,0,0 ) );

	// adding nodes
	QMap<qlonglong, Lua::LuaNode*>::iterator node_iter;
	for ( node_iter = luaGraph->getNodes()->begin();
			node_iter != luaGraph->getNodes()->end();
			++node_iter ) {
		osg::ref_ptr<Data::Node> n = currentGraph->addNode( node_iter.key(), node_iter.value()->getLabel(), nodeType );
		n->setLuaIdentifier( node_iter.value()->getIdentifier() );
		setNodeParams( n, node_iter.value(), osg::Vec4f( 1,1,1,1 ), 8 );

		if ( node_iter.value()->getParams().getValue()["root"] == true ) {
			// connect root node to filesAnchor with invisible edge
			auto metaLink = currentGraph->addEdge( metaEdgeName, n, filesAnchor, currentGraph->getEdgeMetaType(), false );
			metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
			metaLink->setInvisible( true );
		}
		else if ( node_iter.value()->getParams().getValue()["type"] == "directory" ) {
			// connect directory node to filesAnchor with invisible edge
			auto metaLink = currentGraph->addEdge( metaEdgeName, n, filesAnchor, currentGraph->getEdgeMetaType(), false );
			metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
			metaLink->setInvisible( true );
		}
		else if ( node_iter.value()->getParams().getValue()["type"] == "file" ) {
			// connect file node to filesAnchor with invisible edge
			auto metaLink = currentGraph->addEdge( metaEdgeName, n, filesAnchor, currentGraph->getEdgeMetaType(), false );
			metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
			metaLink->setInvisible( true );
		}
		else if ( node_iter.value()->getParams().getValue()["type"] == "function" ) {
			// connect function node to functionsAnchor with invisible edge
			auto metaLink = currentGraph->addEdge( metaEdgeName, n, functionsAnchor, currentGraph->getEdgeMetaType(), false );
			metaLink->setEdgeColor( osg::Vec4( 0,0,0,0 ) );
			metaLink->setInvisible( true );
			metaLink->setEdgeStrength( 0.1f );

			// find max function node size
			float funcNodeSize = node_iter.value()->getFloatParam( "size", 4.0f );
			if ( funcNodeSize > maxFuncNodeSize ) {
				maxFuncNodeSize = funcNodeSize;
			}
		}

	}

	auto nestedEdgeType = currentGraph->addType( Data::GraphLayout::NESTED_EDGE_TYPE );
	auto hierarchyEdgeType = currentGraph->addType( Data::GraphLayout::HIERARCHY_EDGE_TYPE );
	auto arcEdgeType = currentGraph->addType( Data::GraphLayout::ARC_EDGE_TYPE );

	// adding edges
	QMap<qlonglong, Lua::LuaEdge*>::iterator edge_iter;
	for ( edge_iter = luaGraph->getEdges()->begin();
			edge_iter != luaGraph->getEdges()->end();
			++edge_iter ) {
		if ( edge_iter.value()->getIncidences().size() != 2 ) {
			throw new std::runtime_error( "Not a simple graph" );
		}

		// set the correct edge type
		Data::Type* edgeType;
		if ( hierarchyEdges.contains( edge_iter.value()->getLabel() ) ) {
			edgeType = hierarchyEdgeType;
		}
		else if ( arcEdges.contains( edge_iter.value()->getLabel() ) ) {
			edgeType = arcEdgeType;
		}
		else {
			edgeType = nestedEdgeType;
		}

		LuaIncidence* const incid1 = luaGraph->getIncidences()->value( edge_iter.value()->getIncidences()[0] );
		LuaIncidence* const incid2 = luaGraph->getIncidences()->value( edge_iter.value()->getIncidences()[1] );

		osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value( incid2->getEdgeNodePair().second );
		osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value( incid1->getEdgeNodePair().second );
		osg::ref_ptr<Data::Edge> newEdge;
		if ( incid1->getOriented() ) {
			if ( incid1->getOutGoing() ) {
				newEdge = currentGraph->addEdge( edge_iter.key(), edge_iter.value()->getLabel(), dstNode, srcNode, edgeType, true );
			}
			else {
				newEdge = currentGraph->addEdge( edge_iter.key(), edge_iter.value()->getLabel(), srcNode, dstNode, edgeType, true );
			}
		}
		else {
			newEdge = currentGraph->addEdge( edge_iter.key(), edge_iter.value()->getLabel(), srcNode, dstNode, edgeType, false );
		}
		newEdge->setLuaIdentifier( edge_iter.value()->getIdentifier() );
		newEdge->setCamera( camera );
		setEdgeParams( newEdge, edge_iter.value(), osg::Vec4f( 1,1,1,1 ) );
	}

	luaGraph->setObserver( this );


	// delete unwanted node - interface node 'Returns'
	deleteReturnsNode();

	auto resMgr = Manager::ResourceManager::getInstance();

	auto white = resMgr->getMaterial( osg::Vec3( 1.0f, 1.0f, 1.0f ) );
	auto black = resMgr->getMaterial( osg::Vec3( 0.0f, 0.0f, 0.0f ) );
	auto yellow = resMgr->getMaterial( osg::Vec3( 1.0f, 1.0f, 0.0f ) );
	auto red = resMgr->getMaterial( osg::Vec3( 0.941f, 0.502f, 0.502f ) );
	auto green = resMgr->getMaterial( osg::Vec3( 0.565f, 0.933f, 0.565f ) );
	auto orange = resMgr->getMaterial( osg::Vec3( 1.000f, 0.647f, 0.000f ) );



	// load config file
	auto config = Util::ApplicationConfig::get();

	const float FILE_BASE_SIZE = config->getFloatValue( "City.Building.LuaFileBaseSize", DEFAULT_FILE_BASE_SIZE );
	const float FILE_HEIGHT = config->getFloatValue( "City.Building.LuaFileHeight", DEFAULT_FILE_HEIGHT );
	const float NODE_BASE_SIZE = config->getFloatValue( "City.Building.LuaNodeBaseSize", DEFAULT_NODE_BASE_SIZE );
	const float NODE_HEIGHT = config->getFloatValue( "City.Building.LuaNodeHeight", DEFAULT_NODE_HEIGHT );
	const float NODE_MAX_HEIGHT = config->getFloatValue( "City.Building.LuaNodeMaxHeight", DEFAULT_NODE_MAX_HEIGHT );
	const float BALL_SIZE = config->getFloatValue( "City.Ball.DefaultBaseSize", DEFAULT_BALL_SIZE );

	// iterate through all nodes and adjust visual form
	QMap<qlonglong, Lua::LuaNode*>::iterator i;
	for ( i = luaGraph->getNodes()->begin();
			i != luaGraph->getNodes()->end();
			++i ) {
		auto graphNode = currentGraph->getNodes()->value( i.key() );
		if ( graphNode == nullptr ) {
			continue;
		}
		if ( i.value()->getParams().getValue()["root"] == true ) {
			adjustBuildingForNode( graphNode, FILE_BASE_SIZE, FILE_HEIGHT, false, white, "Viewer.Textures.ProjectNode" );
			graphNode->adjustLabelForModule( 50.0f );
		}
		else if ( i.value()->getParams().getValue()["type"] == "directory" ) {
			adjustBuildingForNode( graphNode, FILE_BASE_SIZE, FILE_HEIGHT, false, white, "Viewer.Textures.FolderNode" );
			graphNode->adjustLabelForModule( 50.0f );
		}
		else if ( i.value()->getParams().getValue()["type"] == "file" ) {
			adjustBuildingForNode( graphNode, FILE_BASE_SIZE, FILE_HEIGHT, false, white, "Viewer.Textures.LuaFileNode" );
			graphNode->adjustLabelForModule( 50.0f );
		}
		else if ( i.value()->getParams().getValue()["type"] == "function" ) {
			float funcNodeSize = ( i.value()->getFloatParam( "size", 4.0f ) * NODE_MAX_HEIGHT ) / maxFuncNodeSize;
			adjustBuildingForNode( graphNode, NODE_BASE_SIZE, funcNodeSize, true, red, "" );
		}
		else if ( i.value()->getParams().getValue()["type"] == "global function" ) {
			adjustBuildingForNode( graphNode, NODE_BASE_SIZE, NODE_HEIGHT, true, orange, "" );
		}
		else if ( i.value()->getParams().getValue()["type"] == "local variable" ) {
			adjustBuildingForNode( graphNode, NODE_BASE_SIZE, NODE_HEIGHT, true, yellow, "" );
		}
		else if ( i.value()->getParams().getValue()["type"] == "global variable" ) {
			adjustBuildingForNode( graphNode, NODE_BASE_SIZE, NODE_HEIGHT, true, yellow, "" );
		}
		else if ( i.value()->getParams().getValue()["type"] == "interface" ) {
			adjustBuildingForNode( graphNode, NODE_BASE_SIZE, NODE_HEIGHT, true, green, "" );
		}
		else {
			adjustBallForNode( graphNode, BALL_SIZE, false, black );
		}
	}
}

void Lua::ModuleGraphVisualizer::onUpdate()
{
	std::cout << "ModuleGraph update called" << std::endl;
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

void Lua::ModuleGraphVisualizer::deleteReturnsNode()
{
	Lua::LuaGraph* luaGraph = Lua::LuaGraph::loadGraph();

	QMap<qlonglong, Lua::LuaNode*>::iterator i;
	for ( i = luaGraph->getNodes()->begin();
			i != luaGraph->getNodes()->end();
			++i ) {
		if ( i.value()->getParams().getValue()["type"] == "interface" && i.value()->getLabel() == "Returns" ) {
			auto returnsNode = currentGraph->getNodes()->value( i.key() );

			osg::ref_ptr<Data::Node> moduleNode;
			QMap<qlonglong, osg::ref_ptr<Data::Edge>>* edges = returnsNode->getEdges();
			for ( auto edge : edges->values() ) {
				// first loop to find module node
				if ( edge->getDstNode() == returnsNode ) {
					moduleNode = edge->getSrcNode();
				}
			}

			for ( auto edge : edges->values() ) {
				// find other interface nodes
				if ( edge->getSrcNode() == returnsNode ) {
					returnsNode->removeEdge( edge );
					moduleNode->addEdge( edge );
					edge->setSrcNode( moduleNode );
					// edge->linkNodes(currentGraph->getEdges());
				}
			}
			currentGraph->removeNode( returnsNode );
		}
	}
}


void Lua::ModuleGraphVisualizer::adjustBuildingForNode( osg::ref_ptr<Data::Node> node, float baseSize, float height, bool lieOnGround, osg::ref_ptr<osg::Material> colour, QString textureName )
{
	auto resMgr = Manager::ResourceManager::getInstance();

	auto building = new City::Building();
	building->setLabel( node->AbsNode::getName() );
	building->setBaseSize( baseSize );
	building->setHeight( height );
	building->setLieOnGround( lieOnGround );
	building->setStateSet( new osg::StateSet() );
	building->getStateSet()->setAttribute( colour );
	building->getStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
	if ( !textureName.isEmpty() ) {
		building->getStateSet()->setTextureAttributeAndModes( 0, resMgr->getTexture( Util::ApplicationConfig::get()->getValue( textureName ) ), osg::StateAttribute::ON );
	}
	building->refresh();
	node->setResidence( building );
}

void Lua::ModuleGraphVisualizer::adjustBallForNode( osg::ref_ptr<Data::Node> node, float baseSize, bool lieOnGround, osg::ref_ptr<osg::Material> colour )
{
	auto resMgr = Manager::ResourceManager::getInstance();

	auto ball = new City::Ball();
	ball->setLabel( node->AbsNode::getName() );
	ball->setBaseSize( baseSize );
	ball->setLieOnGround( lieOnGround );
	ball->setStateSet( new osg::StateSet() );
	ball->getStateSet()->setAttribute( colour );
	ball->getStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
	ball->refresh();
	node->setResidence( ball );
}
